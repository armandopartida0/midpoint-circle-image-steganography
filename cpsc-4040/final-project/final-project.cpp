#include <iostream>
#include <memory>
#include <bitset>
#include <fstream>
#include <OpenImageIO/imageio.h>

#include "Image.hpp"

using namespace std;
using namespace OIIO;

struct point
{
    unsigned int x, y;
};

vector<unsigned char> readFile(string filename)
{
    ifstream input(filename, ios::binary);

    vector<unsigned char> data(istreambuf_iterator<char>(input), {});

    return data;
}

void writeFile(vector<unsigned char> &data, string filename)
{
    ofstream output(filename, ios::binary);

    output.write((char*)(&data[0]), data.size() * sizeof(unsigned char));
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

    // Midpoint Circle algorithm
    int x = 0;
    int y = radius;

    int p = (5 / 4) - radius;

    while( x <= y)
    {
        if(p < 0)
        {
            p += (4 * x) + 6;
        }
        else
        {
            p += (2 * (x-y)) + 5;
            y--;
        }

        x++;

        pixelPoints.push_back({center.x + x, center.y + y});
        pixelPoints.push_back({center.x - x, center.y + y});
        pixelPoints.push_back({center.x + x, center.y - y});
        pixelPoints.push_back({center.x - x, center.y - y});
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
void encode(unique_ptr<Image> &img, vector<unsigned char> &data)
{
    // Find coords of pixels that lie on circumference of circle
    vector<point> coords = findMidpointPixels(img);
    int coordsIdx = 0;

    // Modify LSBs on each coords pixel to store data buffer bits
    for(int dataIdx = 0; dataIdx < data.size(); ++dataIdx)
    {
        bitset<8> bits(data.at(dataIdx));

        for(int bitsIdx = 0; bitsIdx < bits.size(); ++bitsIdx)
        {
            placeBitInPixel(bits[bitsIdx], coords.at(coordsIdx), img);
            coordsIdx++;
        }
    }
}

vector<unsigned char> decode(unique_ptr<Image> &img)
{
    // Vector to store data we get from image
    vector<unsigned char> data{};

    // Find coords of pixels that lie on circumference of circle
    vector<point> coords = findMidpointPixels(img);

    int bitsIdx = 0;
    bitset<8> bits{};

    for(int coordsIdx = 0; coordsIdx < coords.size(); ++coordsIdx)
    {
        if(bitsIdx < 8)
        {
            bits[bitsIdx] = img->getPixels()[coords.at(coordsIdx).y * img->getWidth() + coords.at(coordsIdx).x] & 1;
            bitsIdx++;
        }
        else
        {
            bitsIdx = 0;
            data.push_back(static_cast<unsigned char>(bits.to_ulong()));

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

        cout << "Number of pixels available for data: " << points.size() << '\n';

        // Read in file to encode
        vector<unsigned char> data = readFile(argv[3]);
        encode(img, data);

        writeImage(img, "encoded-image.png");
    }
    else if(argv1 == "-d")
    {
        // Read in cover image
        auto img = make_unique<Image>();
        readImage(img, argv[2]);

        // Get points
        vector<point> points = findMidpointPixels(img);

        // Get decoded data and write to file
        vector<unsigned char> decodedData = decode(img);
        writeFile(decodedData, "decoded-file");
    }
    else
    {
        cout << "Invalid option " << argv1 << '\n';
    }

    return 0;
}