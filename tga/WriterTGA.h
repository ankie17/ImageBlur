#pragma once
#include "TGAImage.h"
#include "GaussianBlur.h"

class WriterTGA
{
    public:
      WriterTGA();
      ~WriterTGA();
      void WriteImage(GaussianBlur* blur,TGAImage* image,std::string path);
    private:
      std::string pathForSave;
      void CloseImage(std::ofstream *image);
      void CloseImage(std::ifstream *image);
      void CheckImage();
};