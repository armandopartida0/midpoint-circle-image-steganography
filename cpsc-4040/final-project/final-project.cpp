#include <iostream>
#include <memory>
#include <cmath>
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

vector<point> findMidpointPixels(const unique_ptr<Image> &img)
{
    // Declare vector to hold all of our coords
    vector<point> pixelPoints{};

    // Find center of image
    point center {img->getWidth() / 2, img->getHeight() / 2};
    
    // Find our radius, depends on smaller dimension
    int radius = (img->getWidth() < img->getHeight() 
        ? img->getWidth() : img->getHeight()) / 2;
    
    // Find points on circumference using formula
    for(int i = 0; i <= 360; ++i)
    {
        point temp;
        temp.x = center.x + static_cast<unsigned int>((radius * cos((i * PI) / 180)));
        temp.y = center.y + static_cast<unsigned int>((radius * sin((i * PI) / 180)));
        pixelPoints.push_back(temp);
    }

    return pixelPoints;
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
    // TODO: Find coords of pixels that lie on circumference of circle
    // TODO: Modify LSBs on each pixel to store data buffer bits
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

    return 0;
}