#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <algorithm>
#include "GaussianBlur.h"
#include "TGAImage.h"

GaussianBlur::GaussianBlur(float blurPower)
{
    int value = blurPower*10.0f;
    if(value>10)
       value = 10;
    if(value<0)
       value = 0;
    this->blurPower = value;
    SetupDefaultConfiguration(); // Setting up default configuration for the filter
    sum = 0.0f;
}

GaussianBlur::~GaussianBlur()
{
    std::cout << "[GaussianBlur] Delete filter...\n"
              << std::endl;
    gaussianKernal.clear();
    gaussianKernal.shrink_to_fit();
}

FPixel GaussianBlur::ApplyFilter(TGAImage *image, FPixel pixel)
{
    int verticleImageBound = (KernalHeight - 1) / 2; // Setting boundaries
    int horizontalImageBound = (KernalWidth - 1) / 2;
    float valueR = 0.0f; // Final sum for each pixel chanel to be determined
    float valueG = 0.0f;
    float valueB = 0.0f;
    float valueA = 0.0f;
    for (int kRow = 0; kRow < KernalHeight; kRow++)
    {
        for (int kCol = 0; kCol < KernalWidth; kCol++)
        {
            //To apply blur:
            // Take all pixels around targeted one from FPixel in the radius of blur
            if (kCol + pixel.positionX < image->GetImageWidth() && kRow + pixel.positionY < image->GetImageHeight())
            {
                FPixel p = image->finalPixels[kCol + pixel.positionX][kRow + pixel.positionY];
                // Each pixel chanel should be multiplied by value calculated by Gaussian formula and add 0.8 to make image bright
                p.R *= gaussianKernal[kRow][kCol] + 0.8f;
                p.G *= gaussianKernal[kRow][kCol] + 0.8f;
                p.B *= gaussianKernal[kRow][kCol] + 0.8f;
                p.A *= gaussianKernal[kRow][kCol] + 0.8f;
                valueR += p.R; //Adding value to the final sum
                valueG += p.G;
                valueB += p.B;
            }
        }
    }
    // After neighbor pixels are calculated, mean value of those should be calculated
    valueR /= KernalHeight * KernalWidth;
    valueG /= KernalHeight * KernalWidth;
    valueB /= KernalHeight * KernalWidth;
    valueA /= KernalHeight * KernalWidth;
    pixel.R = valueR; // Apply mean value to given pixel
    pixel.G = valueG;
    pixel.B = valueB;
    pixel.A = valueA;
    return pixel; // Return pixel to the total data
}

void GaussianBlur::SetupDefaultConfiguration()
{
    sigma = 1.0f;
    CreateFilter(); // Create filter
}

void GaussianBlur::ConfigureFilter(float sigma, float blurPower)
{
    this->sigma = sigma;
    int value = blurPower*10.0f;
    if(value>10)
       value = 10;
    if(value<0)
       value = 0;
    this->blurPower = blurPower;
    CreateFilter();
}

void GaussianBlur::SetBlurPower(float blurPower)
{
    int value = blurPower*10.0f;
    if(value>10)
       value = 10;
    if(value<0)
       value = 0;
    this->blurPower = value;
    CreateFilter();
}

void GaussianBlur::CreateFilter()
{
    std::cout << "[GaussianBlur] Create filter...\n"
              << std::endl;

    KernalHeight += blurPower;
    KernalWidth += blurPower;

    gaussianKernal = CreateKernal(KernalWidth, KernalHeight);

    for (int x = 0; x < KernalHeight; x++)
    {
        for (int y = 0; y < KernalWidth; y++)
        {
            // Calcuate value using the formula and storing into vector
            float value = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            gaussianKernal[x][y] = value;
            sum += value;
        }
    }

    // Normalise values in the vector
    for (int i = 0; i < KernalHeight; ++i)
        for (int j = 0; j < KernalWidth; ++j)
            gaussianKernal[i][j] /= sum;

    std::cout << "[GaussianBlur] Filter was create...\n"
              << std::endl;
}
