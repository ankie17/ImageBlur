#pragma once
#include "TGAImage.h"
#include <vector>

class GaussianBlur
{
public:
    GaussianBlur(float blurPower);
    ~GaussianBlur();
    int KernalWidth = 0;
    int KernalHeight = 0;
    void ConfigureFilter(float sigma,float blurPower);
    void SetBlurPower(float blurPower);
    void SetupDefaultConfiguration();
    FPixel ApplyFilter(TGAImage* image,FPixel pixel);
private:
    void CreateFilter();
    auto CreateKernal(int rows,int cols) { return std::vector<std::vector<float>>(rows, std::vector<float>(cols));}
    float gaussianBlur[5][5];
    std::vector<std::vector<float>> gaussianKernal;
    float sigma;
    float sum;
    int blurPower;
    bool isKernelCreate;
};