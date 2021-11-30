/**
 * Image class to store pixel data and other useful functions.
 * Each image will be stored in the RGBA format.
 *
 * Name: Armando Partida-Sanabia
 * Class: CPSC 4040
 * */

#include "Image.hpp"

#define maximum(x, y, z) ((x) > (y)? ((x) > (z)? (x) : (z)) : ((y) > (z)? (y) : (z)))
#define minimum(x, y, z) ((x) < (y)? ((x) < (z)? (x) : (z)) : ((y) < (z)? (y) : (z)))

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

// Manipulation functions
void Image::invertPixels()
{
    for(int i = 0; i < pixels.size(); i++)
    {
        pixels.at(i).r = 255 - pixels.at(i).r;
        pixels.at(i).g = 255 - pixels.at(i).g;
        pixels.at(i).b = 255 - pixels.at(i).b;
    }
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

void Image::RGBToHSV(pixel p, double &h, double &s, double &v)
{
    double red, green, blue;
    double max, min, delta;

    // RGB on 0-1 scale
    red = p.r / 255.0;
    green = p.g / 255.0;
    blue = p.b / 255.0;

    max = maximum(red, green, blue);
    min = minimum(red, green, blue);

    // Value is maximum of RGB
    v = max;

    // Saturation and hue is 0 if value is 0
    if(max == 0)
    {
        s = 0;
        h = 0;
    }
    else
    {
        s = (max - min) / max;
        delta = max - min;

        if(delta == 0)
        {
            h = 0;
        }
        else
        {
            if(red == max)
            {
                h = (green - blue) / delta;
            }
            else if(green == max)
            {
                h = 2.0 + (blue - red) / delta;
            }
            else
            {
                h = 4.0 + (red - green) / delta;
            }

            h = h * 60.0;
            if(h < 0)
            {
                h = h + 360.0;
            }
        }
    }
}

void Image::chromaKeyImage(double minH, double maxH, double minS, double maxS, double minV, double maxV)
{
    for(int i = 0; i < pixels.size(); i++)
    {
        double h, s, v;
        RGBToHSV(pixels[i], h, s, v);

        // Check if it reaches hue threshold
        if(h >= minH && h <= maxH)
        {
            pixels[i].a = 0;
        }
        else
        {
            pixels[i].a = 255;
        }
    }
}

void Image::convolute(std::vector<std::vector<float>> kernel)
{
    int N = kernel.size();

    // Allocate space for reflected kernel matrix
    std::vector<std::vector<float>> reflectedKernel{};
    reflectedKernel.resize(N);
    for(int i = 0; i < N; i++)
    {
        reflectedKernel[i].resize(N);
    }

    // Reflect kernel matrix and scale by magnitude of either positive or negative weight sums
    // Ensures that sum of weights are between -1-1
    float positive_sum = 0;
    float negative_sum = 0;
    float scalar = 0; // Magnitude
    for(int row = 0; row < N; row++)
    {
        for(int col = 0; col < N; col++)
        {
            reflectedKernel[N - row - 1][N - col - 1] = kernel[row][col];
            if(kernel[row][col] > 0)
            {
                positive_sum += kernel[row][col];
            }
            else if(kernel[row][col] < 0)
            {
                negative_sum += kernel[row][col] * -1;
            }
        }
    }

    // Set scalar to magnitude, and some checks
    if(positive_sum > negative_sum)
    {
        scalar = positive_sum;
    }
    else
    {
        scalar = negative_sum;
    }

    if(scalar != 0)
    {
        scalar = 1.0 / scalar;
    }
    else
    {
        scalar = 1.0;
    }

    // Scale reflected kernel by scalar
    for(int row = 0; row < N; row++)
    {
        for(int col = 0; col < N; col++)
        {
            reflectedKernel[row][col] *= scalar;
        }
    }

    //std::cout << "\n";

    // DEBUG - print out reflected kernel matrix
    /*for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            std::cout << reflectedKernel[i][j] << " ";
        }
        std::cout << "\n";
    }
    */

    // Term by term mult
    int offset = (N / 2);
    //std::cout << "Offset: " << offset << "\n";
    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            // Multiply our kernel onto pixel data (if within boundary)
            if(row - offset >= 0 && row + offset < height 
                && col - offset >= 0 && col + offset < width)
            {
                int sumR = 0;
                int sumG = 0;
                int sumB = 0;
                for(int i = 0; i < N; i++)
                {
                    for(int j = 0; j < N; j++)
                    {
                        sumR += pixels[(row - offset + i) * width + (col - offset + j)].r * reflectedKernel[i][j];
                        sumG += pixels[(row - offset + i) * width + (col - offset + j)].g * reflectedKernel[i][j];
                        sumB += pixels[(row - offset + i) * width + (col - offset + j)].b * reflectedKernel[i][j];
                    }
                }

                sumR = abs(sumR);
                sumG = abs(sumG);
                sumB = abs(sumB);

                pixels[row * width + col].r = sumR;
                pixels[row * width + col].g = sumG;
                pixels[row * width + col].b = sumB;
            }
            else
            {
                // If not, set to 0
                pixels[row * width + col].r = 0;
                pixels[row * width + col].g = 0;
                pixels[row * width + col].b = 0;
            }
        }
    }
}