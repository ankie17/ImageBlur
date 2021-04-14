#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <iterator>
#include "TGAImage.h"

TGAImage::TGAImage(std::string imagePathWithName)
{
    imagePath = imagePathWithName;
    OpenTGAImage();
}

TGAImage::~TGAImage() 
{
    std::cout << "[TGAImage] Original image was delete...\n"
                  << std::endl;
    finalPixels.clear();
    finalPixels.shrink_to_fit();
    TGAHeader.clear();
    TGAHeader.shrink_to_fit();
}

FPixel TGAImage::GetImagePixel(int positionX, int positionY)
{
    FPixel p;
    if(positionX<=GetImageWidth() && positionY<=GetImageHeight())
       p = finalPixels[positionX][positionY];
    return p;
}

void TGAImage::OpenTGAImage()
{
    std::ifstream image; // Creating stream for reading image
    image.open(imagePath); // Open image
    CheckImage(&image); // Check whether image is open
}

void TGAImage::CheckImage(std::ifstream *image)
{
    if (image->is_open())
    {
        std::cout << "[TGAImage] Image open...\n"
                  << std::endl;
        ReadImage(image); // If image is successfully opened, start reading process
    }
    else
        std::cout << "[TGAImage] Image not open...\n"
                  << std::endl;
}

void TGAImage::CloseImage(std::ifstream *image)
{
    image->close();
    std::cout << "[TGAImage] Image close...\n"
              << std::endl;
}

void TGAImage::ReadImage(std::ifstream *image)
{
    std::cout << "[TGAImage] Start read image...\n"
              << std::endl;
    std::uint8_t Header[18] = {0}; // Array for image header
    std::vector<std::uint8_t> ImageData; // Vector for storing pixels data
    // DeCompressed and IsCompressed arrays for checking if image is compressed or not
    static std::uint8_t DeCompressed[12] = {0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    static std::uint8_t IsCompressed[12] = {0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    image->read(reinterpret_cast<char *>(&Header), sizeof(Header)); // Read image header
    if (!std::memcmp(DeCompressed, &Header, sizeof(DeCompressed))) // If image in not compressed
    {
        BitsPerPixel = Header[16]; //Get number of bits in image, if 32 then alfa channel is present
        width = Header[13] * 256 + Header[12]; // Image width
        height = Header[15] * 256 + Header[14]; // Image height
        size = ((width * BitsPerPixel + 31) / 32) * 4 * height; // Total image size

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32)) // If neither 24 bits nor 32 stop reading
        {
            std::cout << "[TGAImage] End read image...\n"
                      << std::endl;
            CloseImage(image);
            throw std::invalid_argument("[TGAImage] Invalid image Format. Required: 24 or 32 Bit Image...");
        }
        ImageData.resize(size); // Change vector size to meet image size
        isImageCompressed = false; // Flag for image not compressed
        image->read(reinterpret_cast<char *>(ImageData.data()), size); // Writing pixels data into vector
    }
    else if (!std::memcmp(IsCompressed, &Header, sizeof(IsCompressed))) // If image compressed
    {
        BitsPerPixel = Header[16];
        width = Header[13] * 256 + Header[12];
        height = Header[15] * 256 + Header[14];
        size = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
        {
            std::cout << "[TGAImage] End read image...\n"
                      << std::endl;
            CloseImage(image);
            throw std::invalid_argument("[TGAImage] Invalid image Format. Required: 24 or 32 Bit Image...");
        }

        PixelInfo Pixel = {0}; // Structure for storing image colors
        int CurrentByte = 0;
        std::size_t CurrentPixel = 0;
        isImageCompressed = true;
        std::uint8_t ChunkHeader = {0}; // Chunk header
        int BytesPerPixel = (BitsPerPixel / 8);
        ImageData.resize(width * height * sizeof(PixelInfo));
        //Reading pixels data
        do
        {
            image->read(reinterpret_cast<char *>(&ChunkHeader), sizeof(ChunkHeader));//Reading small part of the image and storing in the chunk
            if (ChunkHeader < 128) // Ih header is not big enough = less then 128
            {
                ++ChunkHeader;// Increase header
                for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)// Reading pixels from chunk
                {
                    image->read(reinterpret_cast<char *>(&Pixel), BytesPerPixel);

                    ImageData[CurrentByte++] = Pixel.B;
                    ImageData[CurrentByte++] = Pixel.G;
                    ImageData[CurrentByte++] = Pixel.R;
                    if (BitsPerPixel > 24)
                        ImageData[CurrentByte++] = Pixel.A;
                }
            }
            else // If header is to big
            {
                ChunkHeader -= 127; // Decreasing the chunk header
                image->read(reinterpret_cast<char *>(&Pixel), BytesPerPixel);// Read pixel

                for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)// Writing colors
                {
                    ImageData[CurrentByte++] = Pixel.B;
                    ImageData[CurrentByte++] = Pixel.G;
                    ImageData[CurrentByte++] = Pixel.R;
                    if (BitsPerPixel > 24)
                        ImageData[CurrentByte++] = Pixel.A;
                }
            }
        } while (CurrentPixel < (width * height));// Repeat util image is fully read
    }
    else // If unsure whether compressed or not close reading
    {
        std::cout << "[TGAImage] End read image...\n"
                  << std::endl;
        CloseImage(image);
        throw std::invalid_argument("[TGAImage] Invalid image Format. Required: 24 or 32 Bit Image...");
    }
    std::cout << "[TGAImage] End read image...\n"
              << std::endl;
    std::cout << "[TGAImage] Read was succes...\n"
              << std::endl;
    CloseImage(image);
    CreatePixelsData(ImageData);
    for (auto data : Header)
        TGAHeader.push_back(data); // Saving image header for later writing image
}

void TGAImage::CreatePixelsData(std::vector<std::uint8_t> data)
{
    std::cout << "[TGAImage] Create pixels data...\n"
              << std::endl;
    std::uint8_t imageChannels = 4;
    std::uint8_t dataChannels = 4;
    int positionX = 0;
    int positionY = 0;
    // Storing location and color of each pixel into FPixel
    // Storing all FPixel structures into finalPixels vector
    finalPixels = CreatePixelsVector(GetImageWidth(),GetImageHeight());
    finalPixels.resize(GetImageWidth() * GetImageHeight());
    for (uint32_t i = 0; i < GetImageWidth() * GetImageHeight(); i++)
    {
        FPixel p;
        for (std::uint8_t b = 0; b < imageChannels; b++)
        {
            std::uint8_t pixel = data[(i * dataChannels) + (b % dataChannels)]; //Pixel information
            if (b % dataChannels == 0)
                p.R = pixel / 255.0f; // Change pixel chanel into float and store
            if (b % dataChannels == 1)
                p.G = pixel / 255.0f;
            if (b % dataChannels == 2)
                p.B = pixel / 255.0f;
            if (b % dataChannels == 3)
            {
                p.A = pixel / 255.0f; // Storing alfa chanel
                p.positionX = positionX; // Storing position
                p.positionY = positionY;
                finalPixels[positionX][positionY] = p;
                positionX++; // Move to next pixel on the right
                if (positionX == GetImageWidth()) // when line is over move to next
                {
                    positionY++;
                    positionX = 0;
                }
            }
        }
    }
    std::cout << "[TGAImage] End create pixels data...\n"
              << std::endl;
}
