/**
 * Image class to store pixel data and other useful functions.
 * Each image will be stored in the RGBA format.
 *
 * Name: Armando Partida-Sanabia
 * Class: CPSC 4040
 * */

#pragma once

#include <iostream>
#include <vector>

struct pixel
{
    unsigned char r, g, b, a;
};

class Image
{
public:
    Image();

    /**
     * Creates black image using specified dimens and channels
     * */
    Image(int channels, int xRes, int yRes);
    
    ~Image();

    /**
     * Loads given image pixel data. Image pixel data will be converted 
     * to RGBA format. If original image did not have an alpha value, 
     * then a dummy alpha value is used (255)
     * 
     * @param p Image pixel data
     * @param channels Number of color channels image pixel
     * data contains
     * @param xRes image resolution on x-axis
     * @param yRes image resolution on y-axis
     * */
    void loadPixelData(std::vector<unsigned char> p, int channels, int xRes, int yRes);

    /**
     * Returns raw pixel data in unsigned char* vector. RGBA format, 4 channels
     * 
     * @return Continous memory array of pixels
     * */
    std::vector<unsigned char> getRawPixels();

    /**
     * Loads given pixel data vector. Vector has pixels stored as pixel struct.
     * 
     * @param p Image pixel data
     * */
    void loadPixels(std::vector<pixel> p);

    /**
     * Returns pixel data in pixels struct format.
     * 
     * @return Vector of pixel structs
     * */
    std::vector<pixel> getPixels();

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

    /**
     * Set the dimensions of the image
     * 
     * @param w width
     * @param h height
     * */
    void setDimens(unsigned int w, unsigned int h);

    /**
     * Flip image vertically
     * */
    void flipVertical();

private:
    std::vector<pixel> pixels{}; // Pixel data, RGBA
    unsigned int numOfChannels{}; // Number of channels in original image
    unsigned int width{};
    unsigned int height{};
};