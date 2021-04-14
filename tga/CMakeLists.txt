cmake_minimum_required(VERSION 3.19)
project(tga)

set(CMAKE_CXX_STANDARD 14)

add_executable(tga main.cpp GaussianBlur.cpp GaussianBlur.h TGAImage.cpp TGAImage.h WriterTGA.cpp WriterTGA.h)