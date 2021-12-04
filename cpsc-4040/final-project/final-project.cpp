#include <iostream>
#include <memory>
#include <cmath>
#include <bitset>
#include <OpenImageIO/imageio.h>

#include "Image.hpp"

using namespace std;
using namespace OIIO;

constexpr double PI = 3.14159265358979323846;

struct point
{
    unsigned int x, y;
};

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

    cout << "Before placing data: " << bitset<8>(p).to_string() << '\n';

    p = (p & ~1) | bit;

    img->getPixels()[coordinate.y * img->getWidth() + coordinate.x] = p;

    cout << "After placing data: " << bitset<8>(p).to_string() << "\n\n";
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
    if(argc != 4)
    {
        cout << "usage (to encode): program -e <cover-image> <file>\n";
        cout << "usage (to decode): program -d <image> <output-file>\n";
        exit(1);
    }

    // DEBUG: Test reading and writing image
    auto img = make_unique<Image>();
    readImage(img, argv[2]);
    //writeImage(img, argv[3]);

    // DEBUG: Test midpoints and print them out
    vector<point> points = findMidpointPixels(img);

    for(int i = 0; i < points.size(); ++i)
    {
        cout << points.at(i).x << " " << points.at(i).y << '\n';
    }

    cout << "Number of pixels available for data: " << points.size() << '\n';

    // Test encode simple letters
    vector<unsigned char> data{'H', 'I', 'S', 'P', 'Y'};

    encode(img, data);

    // Print out result image
    writeImage(img, argv[3]);

    // TEST
    auto testImg = make_unique<Image>();
    readImage(testImg, argv[3]);

    vector<unsigned char> decoded = decode(testImg);

    cout << "First byte: " << decoded.at(0) << '\n';
    cout << "Second byte: " << decoded.at(1) << '\n';
    cout << "Third byte: " << decoded.at(2) << '\n';
    cout << "Fourth byte: " << decoded.at(3) << '\n';
    cout << "Fith byte: " << decoded.at(4) << '\n';

    return 0;
}