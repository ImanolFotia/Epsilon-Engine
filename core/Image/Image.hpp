#pragma once

#include <SOIL.h>
#include <fstream>
#include <Helpers.hpp>
#include <thread>
#include <exception>
#include <functional>

namespace Epsilon
{
    namespace IO
    {
        namespace Image
        {

            constexpr int PNG_SIZE_OFFSET = 16;

            class Image
            {

            public:
                typedef std::shared_ptr<IO::Image::Image> Image_ptr;
                Image()
                {
                    OnLoad = nullptr;
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
                            mData = SOIL_load_image(path, outwidth, outheight, outchannels, SOIL_LOAD_RGBA);

                            if (mData == 0)
                            {
                                mLoaded = true;
                                data = mData;
                                return;
                            }

                            data = mData;
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
                                mData = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);

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

                unsigned char *getData()
                {
                    return mData;
                }

                std::function<void()> OnLoad = nullptr;
                std::shared_ptr<std::thread> th1;

            private:
                bool mLoaded = false;
                unsigned char *mData = nullptr;
                std::string mPath = "";
                int mWidth = 0;
                int mHeight = 0;
            };

            typedef std::shared_ptr<IO::Image::Image> Image_ptr;
        } // namespace Image
    }     // namespace IO
} // namespace Epsilon
