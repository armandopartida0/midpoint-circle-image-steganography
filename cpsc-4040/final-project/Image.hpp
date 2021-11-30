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

struct pixelHSV
{
    unsigned short h, s, v, a;
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
     * Invert image's pixel color data
     * 
     * */
    void invertPixels();

    /**
     * Flip image vertically
     * */
    void flipVertical();

    /**
     * Convert RGB color pixel to HSV and store in
     * reference variables
     * @param p Input RGB pixel
     * @param h Reference to hue variable
     * @param s Reference to saturation variable
     * @param v Reference to value variable
     * */
    void RGBToHSV(pixel p, double &h, double &s, double &v);

    /**
     * Chroma-keys image based on given parameters. Pixel alpha data is changed.
     * @param minH Minimum hue, inclusive
     * @param maxH Maximum hue, inclusive
     * @param minS Minimum saturation, inclusive
     * @param maxS Maximum saturation, inclusive
     * @param minV Minimum value, inclusive
     * @param maxV Maximum value, inclusive
     * */
    void chromaKeyImage(double minH, double maxH, double minS, double maxS, double minV, double maxV);

    /**
     * Applies kernel pass over pixel data
     * @param kernel kernel filter matrix
     * */
    void convolute(std::vector<std::vector<float>> kernel);

private:
    std::vector<pixel> pixels{}; // Pixel data, RGBA
    unsigned int numOfChannels{}; // Number of channels in original image
    unsigned int width{};
    unsigned int height{};
};