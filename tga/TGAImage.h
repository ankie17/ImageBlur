#pragma once
#include <iostream>
#include <vector>

typedef struct FinalPixel
    {
        int positionX;
        int positionY;
        float R;
        float G;
        float B;
        float A;
    } FPixel;

class TGAImage
{
public:
    TGAImage(std::string imagePathWithName);
    ~TGAImage();
    FPixel GetImagePixel(int positionX, int positionY);
    const std::uint32_t GetImageWidth(){return width;};
    const std::uint32_t GetImageHeight(){return height;};
    const std::vector<std::uint8_t> GetImageHeader(){return TGAHeader;};
    std::vector<std::vector<FPixel>> finalPixels;
private:

    typedef union PixelInfo
    {
        std::uint32_t Colour;
        struct
        {
            std::uint8_t R, G, B, A;
        };
    } * PPixelInfo;
    
    std::string imagePath;
    std::uint32_t width, height, size, BitsPerPixel;
    std::vector<std::uint8_t> TGAHeader;
    //std::vector<FPixel> finalPixels;
    bool isImageCompressed;
    void CheckImage(std::ifstream *image);
    void OpenTGAImage();
    void ReadImage(std::ifstream *image);
    void CloseImage(std::ifstream *image);
    void CreatePixelsData(std::vector<std::uint8_t> data);
    auto CreatePixelsVector(int width,int height) {return std::vector<std::vector<FPixel>>(width, std::vector<FPixel>(height));}
};