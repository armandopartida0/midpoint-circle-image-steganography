#include <iostream>
#include <memory>
#include <bitset>
#include <fstream>
#include <cstdint>
#include <OpenImageIO/imageio.h>

#include "Image.hpp"

using namespace std;
using namespace OIIO;

constexpr int CIRCLE_DISTANCE = 2;

struct point
{
    unsigned int x, y;
};

vector<uint8_t> readFile(string filename)
{
    streampos fileSize;
    ifstream input(filename, ios::binary);

    input.seekg(0, ios::end);
    fileSize = input.tellg();
    input.seekg(0, ios::beg);

    vector<uint8_t> data(fileSize);
    input.read(reinterpret_cast<char*>(&data[0]), fileSize);

    input.close();

    return data;
}

void writeFile(vector<uint8_t> &data, string filename)
{
    ofstream output(filename, ios::binary);

    output.write(reinterpret_cast<const char*>(&data[0]), data.size() * sizeof(uint8_t));

    output.close();
}

/**
 * Loads image into memory
 * 
 * @param img Image variable to store data into
 * @param filepath Location of image
 * */
void readImage(unique_ptr<Image> &img, string filepath)
{
    // Open file through OIIO, load into memory
    auto in = ImageInput::open(filepath);
    if(!in)
        return;
    const ImageSpec &spec = in->spec();
    int xres = spec.width;
    int yres = spec.height;
    int channels = spec.nchannels;
    vector<unsigned char> pixels (xres*yres*channels);
    in->read_image (TypeDesc::UINT8, &pixels[0]);
    in->close ();

    // Initialize Image object
    img->loadPixelData(pixels, channels, xres, yres);
}

/**
 * Writes image to file
 * 
 * @param img Image variable that contains image data
 * @param filename Filename for new file
 * */
void writeImage(const unique_ptr<Image> &img, string filename)
{
    std::unique_ptr<ImageOutput> out = ImageOutput::create (filename);
    if (! out)
        return;
    ImageSpec spec (img->getWidth(), img->getHeight(), 
        img->getNumOfChannels(), TypeDesc::UINT8);
    out->open (filename, spec);
    out->write_image (TypeDesc::UINT8, &img->getPixels()[0]);
    out->close ();
}

void midPointAlgorithm(vector<point> &points, unsigned int radius, point center)
{
    // Midpoint Circle algorithm
    int x = 0;
    int y = radius;

    int p = 1 - radius;

    while( x <= y)
    {
        if(p < 0)
        {
            p += (2 * x) + 3;
        }
        else
        {
            p += (2 * (x-y)) + 5;
            y--;
        }

        x++;

        points.push_back({center.x + x, center.y + y});
        points.push_back({center.x - x, center.y + y});
        points.push_back({center.x + x, center.y - y});
        points.push_back({center.x - x, center.y - y});
    }
}

/**
 * Finds points on the circle cirumference found in image
 * 
 * @param img Image used to calculate circle on it
 * @return vector<point> Vector of points on circle cirumference
 */
vector<point> findMidpointPixels(const unique_ptr<Image> &img)
{
    // Declare vector to hold all of our coords
    vector<point> pixelPoints{};

    // Find center of image
    point center {img->getWidth() / 2, img->getHeight() / 2};
    
    // Find our radius, depends on smaller dimension
    int radius = (img->getWidth() < img->getHeight() 
        ? img->getWidth() : img->getHeight()) / 2;

    radius -= 1; // Make sure the circle stays within image

    // Lets do concentric circles, for more storage
    int maxNumOfCircles = 0;
    int tempRadius = radius;
    while(tempRadius > 0)
    {
        maxNumOfCircles++;
        tempRadius -= CIRCLE_DISTANCE;
    }

    //cout << "Max number of circles available: " << maxNumOfCircles << '\n';

    for(size_t i = 0; i < maxNumOfCircles; ++i)
    {
        midPointAlgorithm(pixelPoints, radius, center);

        // Keep circles distance
        radius -= CIRCLE_DISTANCE;
    }

    return pixelPoints;
}

/**
 * Places bit into LSB of pixel
 * 
 * @param bit Bit to be placed
 * @param coordinate Coordinate of pixel
 * @param img Image that contains pixel
 */
void placeBitInPixel(int bit, point coordinate, unique_ptr<Image> &img)
{
    unsigned char p = img->getPixels()[coordinate.y * img->getWidth() + coordinate.x];

    p = (p & ~1) | bit;

    img->getPixels()[coordinate.y * img->getWidth() + coordinate.x] = p;
}

/**
 * Encodes bits from data buffer into cover image using 
 * midpoint circle approach
 * 
 * @param img Cover image
 * @param data Data buffer
 */
void encode(unique_ptr<Image> &img, vector<uint8_t> &data)
{
    // Find coords of pixels that lie on circumference of circle
    vector<point> coords = findMidpointPixels(img);

    // Check if we have enough pixels to store data
    if(data.size() > ((coords.size() - 32) / 8))
    {
        cout << "Not enough pixels to store data.\n";
        cout << "Data size (in bytes): " << data.size() << '\n';
        cout << "Available data in picture (in bytes): " << ((coords.size() - 32) / 8) << '\n';
        exit(1);
    }

    // Modify LSBs on each coords pixel to store data buffer bits
    // Since we used some bits already, start at the correct pixel
    size_t coordsIdx = 32;
    for(size_t dataIdx = 0; dataIdx < data.size(); ++dataIdx)
    {
        bitset<8> bits(data.at(dataIdx));

        for(size_t bitsIdx = 0; bitsIdx < bits.size(); ++bitsIdx)
        {
            placeBitInPixel(bits[bitsIdx], coords.at(coordsIdx), img);
            coordsIdx++;
        }
    }

    // We have to use the first 32 bits to store how many coords used
    uint32_t coordsUsed = static_cast<uint32_t>(coordsIdx + 1);
    size_t coordsUsedIdx = 0;
    bitset<32> bits(coordsUsed);
    for(size_t bitsIdx = 0; bitsIdx < bits.size(); ++bitsIdx)
    {
        placeBitInPixel(bits[bitsIdx], coords.at(coordsUsedIdx), img);
        coordsUsedIdx++;
    }
}

vector<uint8_t> decode(unique_ptr<Image> &img)
{
    // Vector to store data we get from image
    vector<uint8_t> data{};

    // Find coords of pixels that lie on circumference of circle
    vector<point> coords = findMidpointPixels(img);

    // We have to use check the first 16 bits to see how many coordinates were used
    bitset<32> coordsUsed_b{};
    for(size_t i = 0; i < coordsUsed_b.size(); ++i)
    {
        coordsUsed_b[i] = img->getPixels()[coords.at(i).y * img->getWidth() + coords.at(i).x] & 1;
    }

    uint32_t numCoordsUsed = static_cast<uint32_t>(coordsUsed_b.to_ulong());

    // Start coords at 32
    size_t bitsIdx = 0;
    bitset<8> bits{};
    for(size_t coordsIdx = 32; coordsIdx < numCoordsUsed; ++coordsIdx)
    {
        if(bitsIdx < bits.size())
        {
            bits[bitsIdx] = img->getPixels()[coords.at(coordsIdx).y * img->getWidth() + coords.at(coordsIdx).x] & 1;
            bitsIdx++;
        }
        else
        {
            bitsIdx = 0;
            data.push_back(static_cast<uint8_t>(bits.to_ulong()));

            // Don't skip a bit
            coordsIdx--;
        }
    }

    return data;
}

int main(int argc, char** argv)
{
    // Check args
    if((argc != 4) && (argc != 3))
    {
        cout << "usage (to encode): program -e <cover-image> <file>\n";
        cout << "usage (to decode): program -d <image>\n";
        exit(1);
    }

    string argv1 = argv[1];
    if(argv1 == "-e")
    {
        // Read in cover image
        auto img = make_unique<Image>();
        readImage(img, argv[2]);

        // Get points
        vector<point> points = findMidpointPixels(img);

        // Read in file to encode
        vector<uint8_t> data = readFile(argv[3]);
        encode(img, data);

        writeImage(img, "encoded-image.png");
    }
    else if(argv1 == "-d")
    {
        // Read in cover image
        auto img = make_unique<Image>();
        readImage(img, argv[2]);

        // Get decoded data and write to file
        vector<uint8_t> decodedData = decode(img);
        writeFile(decodedData, "decoded-file");
    }
    else
    {
        cout << "Invalid option " << argv1 << '\n';
    }

    return 0;
}