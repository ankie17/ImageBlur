#include <iostream>
#include "TGAImage.h"
#include "WriterTGA.h"
#include "GaussianBlur.h"


int main()
{
    std::string pathOriginalImage;
    std::string pathNewImage;
    float blurPower;

    std::cout << "[Main] Write image path with name(.tga):" << "\n" << std::endl;
    std::cin >> pathOriginalImage;
    std::cout << "[Main] Write path for save image with name(.tga):" << "\n" << std::endl;
    std::cin >> pathNewImage;
    std::cout << "[Main] Write blur power:" << "\n" << std::endl;
    std::cin >> blurPower;

    TGAImage image {pathOriginalImage}; // Read image from given path
    WriterTGA writerTGA; // Creating object for writing new image
    GaussianBlur gaussianBlur {blurPower}; // Creating object with blur algorithm
    writerTGA.WriteImage(&gaussianBlur,&image,pathNewImage); // Writing new blured image
    writerTGA.~WriterTGA();// Deleting an object for writing
    std::cout << "[Main] End work program..." << "\n" << std::endl;
    return 0;
}