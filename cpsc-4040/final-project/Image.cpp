/**
 * Image class to store pixel data and other useful functions.
 *
 * Name: Armando Partida-Sanabia
 * Class: CPSC 4040
 * */

#include "Image.hpp"

Image::Image(){}

Image::Image(int channels, int w, int h)
{
    pixels.assign(w * h * channels, 0);
    numOfChannels = channels;
    width = w;
    height = h;
}

Image::~Image(){}

void Image::loadPixelData(std::vector<unsigned char> p, int channels, int w, int h)
{
    pixels = p;
    numOfChannels = channels;
    width = w;
    height = h;
}

std::vector<unsigned char>& Image::getPixels()
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