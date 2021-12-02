/**
 * Image class to store pixel data and other useful functions.
 * Each image will be stored in the RGBA format.
 *
 * Name: Armando Partida-Sanabia
 * Class: CPSC 4040
 * */

#include "Image.hpp"

Image::Image(){}

Image::Image(int channels, int xRes, int yRes)
{
    numOfChannels = channels;
    width = xRes;
    height = yRes;

    // Create black image
    for(int i = 0; i < width * height; i++)
    {
        pixel temp = { 0, 0, 0, 0 };
        pixels.push_back(temp);
    }
}

Image::~Image(){}

void Image::loadPixelData(std::vector<unsigned char> p, int channels, int w, int h)
{
    for(int i = 0; i < p.size(); i++)
    {
        pixel temp{};

        if(channels == 1)
        {
            unsigned char greyColor = p.at(i);
            temp = { greyColor, greyColor, greyColor, 255 };
        }
        else if(channels == 3)
        {
            temp = { p.at(i), p.at(i+1), p.at(i+2), 255 };
            i += 2;
        }
        else
        {
            temp = { p.at(i), p.at(i+1), p.at(i+2), p.at(i+3) };
            i += 3;
        }

        pixels.push_back(temp);
    }

    numOfChannels = channels;
    width = w;
    height = h;
}

std::vector<unsigned char> Image::getRawPixels()
{
    std::vector<unsigned char> pixelsRaw;

    for(int i = 0; i < pixels.size(); i++)
    {
        pixelsRaw.push_back(pixels.at(i).r);
        pixelsRaw.push_back(pixels.at(i).g);
        pixelsRaw.push_back(pixels.at(i).b);
        pixelsRaw.push_back(pixels.at(i).a);
    }

    return pixelsRaw;
}

void Image::loadPixels(std::vector<pixel> p)
{
    pixels = p;
}

std::vector<pixel> Image::getPixels()
{
    return pixels;
}

unsigned int Image::getNumOfChannels()
{
    return numOfChannels;
}

unsigned int Image::getWidth()
{
    return width;
}

unsigned int Image::getHeight()
{
    return height;
}

void Image::setDimens(unsigned int w, unsigned int h)
{
    width = w;
    height = h;
}

void Image::flipVertical()
{
    std::vector<pixel> flippedPixels(width * height * 4);
    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            flippedPixels.at((height - row - 1) * width + col).r = pixels.at(row * width + col).r;
            flippedPixels.at((height - row - 1) * width + col).g = pixels.at(row * width + col).g;
            flippedPixels.at((height - row - 1) * width + col).b = pixels.at(row * width + col).b;
            flippedPixels.at((height - row - 1) * width + col).a = pixels.at(row * width + col).a;
        }
    }

    pixels = flippedPixels;
}