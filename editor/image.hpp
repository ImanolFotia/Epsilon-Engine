#pragma once

#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <threadmanager.hpp>

class Image
{
public:
    Image() = default;
    Image(std::string);

    ~Image(){}

    void Load(std::string, int id = 0);
    std::shared_ptr<std::thread> AsyncLoad(std::string);

    typedef unsigned char uchar;
    uchar* getPixels(){ return mData; }

    std::string getPath(){ return mPath; }


    int getWidth(){ return mWidth; }
    int getHeight(){ return mHeight; }
    int getChannels(){ return mChannels; }
    int getId() { return this->mId; }

    bool isLoaded(){ return mLoaded; }

    bool Destroy();

    static void DispatchLoad(Image* ptr, std::string path);
    static void DispatchLoadId(Image* ptr, std::string path, int id); //added id parameter for cubemap loading

    enum IMAGE_FORMAT
    {
        PNG = 0,
        JPG = 1,
        JPEG = 1,
        TGA = 2,
        BMP = 3,
        DDS = 4,
        NO_EXTENSION = 99
    };

    IMAGE_FORMAT getFormat(){return mFormat; }

    std::function<void(void)> mOnLoad = nullptr;
    std::function<void(int)> mOnLoadCubemap = nullptr;

public:
    void onLoadCallback(std::function<void(void)> f){ this->mOnLoad = f; }
    void onLoadCallbackCubemap(std::function<void(int)> f){ this->mOnLoadCubemap = f; }

private:
    IMAGE_FORMAT ExtractFormat(std::string);


    uchar* mData = nullptr;

    IMAGE_FORMAT mFormat;

    std::string mPath;

    int mWidth = 0;
    int mHeight = 0;
    int mChannels = 0;
    int mId = -1;

    bool mLoaded = false;
    std::mutex m;
};
