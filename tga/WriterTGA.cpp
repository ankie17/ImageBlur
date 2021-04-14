#include "WriterTGA.h"
#include "TGAImage.h"
#include "GaussianBlur.h"
#include <fstream>
#include <iostream>

WriterTGA::WriterTGA(){}
WriterTGA::~WriterTGA()
{
    std::cout << "[WriterTGA] Writer was delete...\n"
              << std::endl;
}
void WriterTGA::WriteImage(GaussianBlur* blur,TGAImage* image,std::string path)
{
    pathForSave = path;
    std::cout << "[WriterTGA] Start create new image...\n"
              << std::endl;
    std::ofstream newImage(pathForSave,std::ios::binary);
    std::uint8_t Header[18] = {0};
    for(int i = 0; i<image->GetImageHeader().size();i++) // Taking image header
        Header[i] = image->GetImageHeader()[i];
    newImage.write(reinterpret_cast<char *>(&Header), sizeof(Header)); // Storing header for new image
    //НWriting pixels from finalPixels
    for (uint32_t y = 0; y < image->GetImageHeight(); y++)
    {
        for (uint32_t x = 0; x < image->GetImageWidth(); x++)
        {
            FPixel newPixel;
            newPixel = blur->ApplyFilter(image,image->finalPixels[x][y]); // Taking structure where pixel data is stored
            // Change back to bytes and write into file
            uint8_t r = newPixel.R * 255.0f;
            newImage.write(reinterpret_cast<char *>(&r),sizeof(r));
            uint8_t g = newPixel.G * 255.0f;
            newImage.write(reinterpret_cast<char *>(&g),sizeof(g));
            uint8_t b = newPixel.B * 255.0f;
            newImage.write(reinterpret_cast<char *>(&b),sizeof(b));
            uint8_t a = newPixel.A * 255.0f;
            newImage.write(reinterpret_cast<char *>(&a),sizeof(a));
        }
    }
    image->~TGAImage(); // Delete original image data
    blur->~GaussianBlur(); // Delete blured image
    std::cout << "[WriterTGA] End create new image...\n"
              << std::endl;
    CloseImage(&newImage);// Close chanel
    CheckImage();// Checking image
}
void WriterTGA::CloseImage(std::ofstream *image)
{
    image->close();
    std::cout << "[WriterTGA] Image close...\n"
              << std::endl;
}
void WriterTGA::CloseImage(std::ifstream *image)
{
    image->close();
    std::cout << "[WriterTGA] Image close...\n"
              << std::endl;
}
void WriterTGA::CheckImage()
{
    std::ifstream image;
    image.open(pathForSave);
    if (image.is_open())
        std::cout << "[WriterTGA] Image open. New image write succes...\n"
                  << std::endl;
    else
        std::cout << "[WriterTGA] Image not open. New image write fail...\n"
                  << std::endl;
    CloseImage(&image);
}