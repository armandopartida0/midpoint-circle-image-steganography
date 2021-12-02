#include <iostream>
#include <memory>
#include <OpenImageIO/imageio.h>

#include "Image.hpp"

using namespace std;
using namespace OIIO;

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
    /*auto img = make_unique<Image>();
    readImage(img, argv[2]);
    writeImage(img, argv[3]);*/

    return 0;
}