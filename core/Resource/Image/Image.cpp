/*#include <image.hpp>
#include <iostream>
#include <mutex>
#include <chrono>
#include <random>
#include <memory>
#include <Includes.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <deps/stb_image.h>
//#include <include/Log.hpp>
#endif
// Constructor that loads and image given an absolute path
Image::Image(std::string path)
{
    this->mPath = path;
    int x,y,n;
    this->mFormat = ExtractFormat(path);
    //stbi_set_flip_vertically_on_load(true);
    mData = stbi_load(path.c_str(), &x, &y, &n, 0);

    this->mWidth = x;
    this->mHeight = y;
    this->mChannels = n;

    mLoaded = true;
}

void Image::Load(std::string path, int id)
{
    this->mId = id;
    try{
        std::lock_guard<std::mutex> lock(m);
        this->mPath = path;
        int x = 0,y = 0,n = 0;
        this->mFormat = ExtractFormat(path);
        bool empty = true;
        int attempts = 0;
        while(empty){

            path = replace(path, '\\', '/');
            mData = stbi_load(path.c_str(), &x, &y, &n, 0);
            //m.unlock();
            if(mData != nullptr) {

                std::cout << "STBI could load image " << path << std::endl;
                empty = false;
                break;
            }
            else {
                    mData = stbi_load("C:/Users/Imanol/Documents/Code/C++/Level Editor QT/build-LevelEditor-MingW64-Debug/assets/textures/checkers.png", &x, &y, &n, 0);
                    mLoaded = true;
                    empty = false;
                    break;
            }
            attempts++;
        }

        this->mWidth = x;
        this->mHeight = y;
        this->mChannels = n;

        mLoaded = true;
    } catch(...)
    {
        std::exception_ptr p = std::current_exception();
        std::clog <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
    }
}

void Image::DispatchLoad(Image* ptr, std::string path){

    static_cast<Image*>(ptr)->Load(path);
}

void Image::DispatchLoadId(Image* ptr, std::string path, int id){

    static_cast<Image*>(ptr)->Load(path, id);
}

std::shared_ptr<std::thread> Image::AsyncLoad(std::string path)
{
    try{
        std::shared_ptr<std::thread> t = (std::shared_ptr<std::thread>)( new std::thread([=] { Load(path); }));
        return t;

    } catch(...)
    {
#ifdef __GNUC__
        std::exception_ptr p = std::current_exception();
        std::clog <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
#elif defined(_MSC_VER)
        std::exception_ptr p = std::current_exception();
        std::clog << "Caught exception" << std::endl;
#endif

    }
}

Image::IMAGE_FORMAT Image::ExtractFormat(std::string path)
{
    std::string ext;

    std::size_t found = path.find(".");

    if(found == std::string::npos)
        return IMAGE_FORMAT::NO_EXTENSION;

    std::size_t EXTL = path.length() - found;

    for(unsigned int i = path.length()-EXTL ; i < path.length() ; i++) {
        ext += path.at(i);
    }

    if(ext == "png" || ext == "PNG"){
        return IMAGE_FORMAT::PNG;
    }
    else if(ext == "jpg" || ext == "JPG")
    {
        return IMAGE_FORMAT::JPG;
    }
    else if(ext == "jpeg" || ext == "JPEG")
    {
        return IMAGE_FORMAT::JPEG;
    }
    else if(ext == "tga" || ext == "TGA")
    {
        return IMAGE_FORMAT::TGA;
    }
    else if(ext == "bmp" || ext == "BMP")
    {
        return IMAGE_FORMAT::BMP;
    }
    else if(ext == "dds" || ext == "DDS")
    {
        return IMAGE_FORMAT::DDS;
    }
    else
    {
        return IMAGE_FORMAT::NO_EXTENSION;
    }
}

bool Image::Destroy(){
    stbi_image_free(mData);
    return true;
}
*/