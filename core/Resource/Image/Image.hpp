#pragma once

#include <pch.hpp>
#include <Helpers.hpp>

#include "../Resource.hpp"
#include <Error.hpp>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

namespace Epsilon
{
    namespace Resources
    {
        namespace Image
        {

            constexpr int PNG_SIZE_OFFSET = 16;

            class Image : public Resource<ResourceType::IMAGE>
            {

            public:
                typedef std::shared_ptr<Image> Image_ptr;
                Image()
                {
                    OnLoad = nullptr;
                    mData = std::make_shared<unsigned char>();
                    mData = nullptr;
                }

                //Image(char* path) : mPath(path) {}

                void LoadFile(const char *path, unsigned char *&data, int *outwidth, int *outheight, int *outchannels)
                {
                    try
                    {

                        if (std::string(path).find("png") != std::string::npos)
                            getSizePNG(path, outwidth, outheight, outchannels);

                        th1 = (std::shared_ptr<std::thread>)new std::thread([&]() -> void {
                            mPath = path;
                            mData.reset(stbi_load(path, outwidth, outheight, outchannels, 4));

                            if (mData == 0)
                            {
                                mLoaded = true;
                                data = mData.get();
                                return;
                            }

                            data = mData.get();
                            mLoaded = true;
                        });

                        if (std::string(path).find("png") != std::string::npos)
                        {
                        }
                        else
                        {
                            th1->join();
                        }
                    }
                    catch (std::exception &e)
                    {
                        std::cout << "Exception detected in IO::Image::Image::LoadFile:\nArgument: " << path << " Code: " << e.what() << std::endl;
                    }
                }

                void LoadFile(const char *path)
                {
                    try
                    {
                        th1 = (std::shared_ptr<std::thread>)new std::thread([this, path]() -> void {
                            try
                            {

                                std::mutex mtx;
                                mtx.try_lock();
                                int width = 16, height = 16, channels = 4;

                                //if(std::string(path).find("png") != std::string::npos)
                                //    getSizePNG(path, &width, &height, &channels);

                                mPath = path;
                                std::cout << "Loading " << path << std::endl;
                                mData.reset(stbi_load(path, &width, &height, &channels, 4));

                                if (mData == 0)
                                {
                                    mLoaded = true;
                                    return;
                                }
                                mLoaded = true;

                                std::cout << "Finished loading: " << path << std::endl;
                                mtx.unlock();
                            }
                            catch (std::exception &e)
                            {
                                std::cout << "Exception detected in IO::Image::Image::LoadFile::Thread (No arguments): \nArgument: " << path << " Code: " << e.what() << std::endl;
                            }
                        });

                        //th1->join();
                        //OnLoad();

                        Load();
                    }
                    catch (std::exception &e)
                    {
                        std::cout << "Exception detected in IO::Image::Image::LoadFile (No arguments): \nArgument: " << path << " Code: " << e.what() << std::endl;
                    }
                }

                void Load()
                {
                    if (OnLoad != nullptr && th1->joinable())
                    {
                        th1->join();
                        OnLoad();
                    }
                }

                void getSizePNG(const char *p, int *w, int *h, int *c)
                {
                    try
                    {
                        std::ifstream in(p, std::ios::binary);
                        uint32_t width = 0, height = 0;

                        in.seekg(PNG_SIZE_OFFSET, in.beg);
                        in.read((char *)&width, sizeof(char) * 4);
                        in.read((char *)&height, sizeof(char) * 4);

                        in.seekg(25, in.beg);
                        uint32_t color = 0;
                        in.read((char *)&color, sizeof(char) * 1);
                        in.close();

                        auto channels = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
                        *c = channels;

                        width = Helpers::SwapEndian<uint32_t>(width);
                        height = Helpers::SwapEndian<uint32_t>(height);
                        //std::cout << "Width: " << width << " Height: " << height << " Channels: " << channels << std::endl;

                        *w = (int)width;
                        *h = (int)height;

                        mWidth = width;
                        mHeight = height;

                        //in.close();
                    }
                    catch (std::exception &e)
                    {
                        std::cout << e.what() << std::endl;
                    }
                }

                void getSizeBMP(const char *p, int *w, int *h)
                {
                }

                void getSizeTGA(const char *p, int *w, int *h)
                {
                }

                void getSizeDDS(const char *p, int *w, int *h)
                {
                }
                bool isLoaded()
                {
                    return mLoaded;
                }

                void onLoad(std::function<void()> f)
                {
                    OnLoad = f;
                }

                std::shared_ptr<unsigned char> getData()
                {
                    return mData;
                }

                std::variant<std::monostate, std::shared_ptr<unsigned char>, Error> Get()
                {
                    //...
                    if (mData != nullptr)
                    {
                        return mData;
                    }
                    else if (mError.Type() != NOT_SPECIFIED)
                    {
                        return mError;
                    }
                    else {

                    }
                }

                std::function<void()> OnLoad = nullptr;
                std::shared_ptr<std::thread> th1;

            private:
                bool mLoaded = false;
                std::string mPath = "";
                int mWidth = 0;
                int mHeight = 0;
                Error mError;
            };

            typedef std::shared_ptr<Image> Image_ptr;
        } // namespace Image
    }     // namespace Resources
} // namespace Epsilon
