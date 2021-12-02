/**
 * Image class to store pixel data and other useful functions.
 *
 * Name: Armando Partida-Sanabia
 * Class: CPSC 4040
 * */

#pragma once

#include <iostream>
#include <vector>

class Image
{
public:
    Image();

    /**
     * Creates black image using specified dimens and channels
     * */
    Image(int channels, int w, int h);
    
    ~Image();

    /**
     * Loads given image pixel data.
     * 
     * @param p Image pixel data
     * @param channels Number of channels that data contains
     * @param xRes image resolution on x-axis
     * @param yRes image resolution on y-axis
     * */
    void loadPixelData(std::vector<unsigned char> p, int channels, int xRes, int yRes);

    /**
     * Returns reference to pixel data
     * 
     * @return Continous memory array of pixels
     * */
    std::vector<unsigned char>& getPixels();

    /**
     * Return number of channels in original image
     * 
     * @return number of channels
     * */
    unsigned int getNumOfChannels();

    /**
     * Return width of image
     * 
     * @return width of image
     * */
    unsigned int getWidth();

    /**
     * Return height of image
     * 
     * @return height of image
     * */
    unsigned int getHeight();

private:
    std::vector<unsigned char> pixels{}; // Pixel data in original image
    unsigned int numOfChannels{}; // Number of channels in original image
    unsigned int width{}; // Width of original image
    unsigned int height{}; // Height of original image
};