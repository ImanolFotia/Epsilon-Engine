///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <mutex>
#include <memory>
#include <SOIL.h>
#include <Image/Image.hpp>
#include <ProgramData.h>
#include <Includes.h>
#include <Image/Image.hpp>
#include <Log.h>
using namespace std;

class eTexture {

public:
    eTexture(const char* TexName, GLenum wrap = GL_REPEAT, GLenum type = GL_TEXTURE_2D, GLuint filtering = -1) {
        ProgramData DATA;
        int channels;
        std::string ext = Helpers::getExtension(TexName);
        //if(ext.find("png") != std::string::npos)
        path = ("materials/" + std::string(TexName)).c_str();
        loadFile(path.c_str(), image, &width, &height, &channels);
        createGLTexture(image, TexName, wrap, type, filtering);
        //cout << path << " Extension: " << ext << endl;
        /*
                std::cout << "Time to load from file to RAM: ";*/
/*
        mImage = (std::shared_ptr<IO::Image::Image>) new IO::Image::Image();

        glGenTextures(1, &texture);
        if(ext.find("png") != std::string::npos) {
            mImage->getSizePNG(path.c_str(), &width, &height, &channels);
            //width /= 2;
            //height /= 2;
            //channels = 4;
            createNullTexture(TexName, wrap, type, filtering);
            ispng = true;
        }*/

        //unsigned char* image;
        //mImage = new IO::Image::Image();

        //if(ext.find("png") == std::string::npos) {
            //mImage->LoadFile(path.c_str(), image, &width, &height, &channels);
            //createGLTexture(image, TexName, wrap, type, filtering);
       /* } else {
            mImage->onLoad([&]() -> void {
                image = mImage->getData();
                createGLTexture(image, TexName, wrap, type, filtering);
            });

            mImage->LoadFile(path.c_str());
            //mImage->Load();
            //std::cout << "LoadFile " << path << std::endl;


        }*/
    }

    eTexture(std::vector<std::string> CubeMapPath) {
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        int channels;
        unsigned char* image;

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        for(GLuint i = 0; i < CubeMapPath.size(); i++) {
            image = SOIL_load_image(CubeMapPath[i].c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
            std::cout << CubeMapPath[i] << std::endl;
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
        //std::cout << "CubeMapID" << texture << std::endl;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void createNullTexture(const char* TexName, GLenum wrap = GL_REPEAT, GLenum type = GL_TEXTURE_2D, GLuint filtering = -1)
    {
        //std::cout << "createNullTexture " << path << std::endl;
        ProgramData DATA;
        glGenTextures(1, &texture);
        glBindTexture(type, texture);

        if(isNormal(TexName)) {
            if(type == GL_TEXTURE_2D) { //GL_COMPRESSED_RGBA_S3TC_DXT5_EXT

                glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            } else if(type == GL_TEXTURE_1D) {
                if(DATA.COMPRESSED_TEXTURES) {
                    glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                } else {
                    glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                }
            }

        } else {
            if(type == GL_TEXTURE_2D)//GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
                if(DATA.COMPRESSED_TEXTURES) {
                    glTexImage2D(type, 0, GL_TEXTURE_COMPRESSION_S3TC_DXT5_SRGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

                } else
                    glTexImage2D(type, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            else if(type == GL_TEXTURE_1D)
                glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
        glGenerateMipmap(type);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
        if(filtering  == (unsigned int)-1) {
            if(DATA.ANISOTROPY <= 0) {
                glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            } else {
                glTexParameterf(type, GL_TEXTURE_MAX_ANISOTROPY_EXT, DATA.ANISOTROPY);
            }
        } else {
            glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        if(!texture) {
            cout << "no se pudo crear" << TexName << endl;
            Global::Log::WriteToLog("Texture: " + std::string(TexName) + " failed to load.");
            return;
        }
        glBindTexture(type, 0);
    }

    void createGLTexture(unsigned char* image, const char* TexName, GLenum wrap = GL_REPEAT, GLenum type = GL_TEXTURE_2D, GLuint filtering = -1) {
        //std::mutex mtx;
        //mtx.lock();

        //std::cout << "Fill Texture Data " << path << std::endl;
        ProgramData DATA;
        if(!image)
            return;
        /*
        std::cout << "Time to load from RAM to GPU: ";*/
        glGenTextures(1, &texture);
        glBindTexture(type, texture);

        if(isNormal(TexName)) {
            if(type == GL_TEXTURE_2D) { //GL_COMPRESSED_RGBA_S3TC_DXT5_EXT

                glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            } else if(type == GL_TEXTURE_1D) {
                if(DATA.COMPRESSED_TEXTURES) {
                    glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                } else {
                    glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                }
            }

        } else {
            if(type == GL_TEXTURE_2D)//GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
                if(DATA.COMPRESSED_TEXTURES) {
                    glTexImage2D(type, 0, GL_TEXTURE_COMPRESSION_S3TC_DXT5_SRGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

                } else
                    glTexImage2D(type, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

            else if(type == GL_TEXTURE_1D)
                glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        glGenerateMipmap(type);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
        if(filtering  == -1) {
            if(DATA.ANISOTROPY <= 0) {
                glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            } else {
                glTexParameterf(type, GL_TEXTURE_MAX_ANISOTROPY_EXT, DATA.ANISOTROPY);
            }
        } else {
            glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        if(!texture) {
            cout << "no se pudo crear" << TexName << endl;
            Global::Log::WriteToLog("Texture: " + std::string(TexName) + " failed to load.");
            return;
        }
        SOIL_free_image_data(image);
        glBindTexture(type, 0);

        Global::Log::WriteToLog("Texture: " + std::string(TexName) + " Loaded.");
        //mtx.unlock();
    }

    void loadFile(const char* path, unsigned char* &data, int *outwidth, int *outheight, int *outchannels) {
        data = SOIL_load_image(path, outwidth, outheight, outchannels, SOIL_LOAD_RGBA);
    }

    bool isNormal(const char* TexName) {
        string name = string(TexName);
        std::size_t found2 = name.find("_s");
        std::size_t found1 = name.find("_n");
        std::size_t found3 = name.find("specular");
        std::size_t found4 = name.find("normal");
        std::size_t found5 = name.find("ddn");
        std::size_t found6 = name.find("nrm");
        std::size_t found7 = name.find("Normal");

        std::size_t npos = std::string::npos;

        return          found1 != npos ||
                        found2 != npos ||
                        found3 != npos ||
                        found4 != npos ||
                        found5 != npos ||
                        found6 != npos ||
                        found7 != npos;
    }

    void Destroy() {
       // if(mImage->th1->joinable())
        //    mImage->th1->join();
        glDeleteTextures(1, &texture);
    }

    virtual ~eTexture() {
        std::cout << "eTexture Destroyed: " << path << std::endl;

    }

public:

    GLuint getTextureID(void) {
        setRequestCount();
        //checkLoading();
        return texture;
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    const char* getPath() {
        return path.c_str();
    }

    void setRequestCount() {
        m_TimesUsed++;
    }

    void resetRequestCount() {
        m_TimesUsed = 0;
    }

    int getTimesUsed() {
        return m_TimesUsed;
    }

    long toHash() {
        return std::hash<std::string> {}(path);
    }

    void checkLoading() {
        try {
            if(ispng) {
                if(!mLoaded) {
                    //std::cout << "Loading inside texture " << path << std::endl;
                    if(mImage != nullptr) {
                        //std::cout << "checkLoading " << path << std::endl;
                        if(mImage->isLoaded()) {
                            if(mImage->OnLoad != nullptr) {
                                if(mImage->th1->joinable()) {
                                    mImage->th1->join();
                                    mImage->Load();
                                    //std::cout << "Joining and loading " << path << std::endl;

                                    //unsigned char* image = mImage->getData();
                                    //this->createGLTexture(image, path.c_str());
                                    //mImage->OnLoad();
                                }
                            }
                            mLoaded = true;
                        }
                    }
                }
            }
        } catch(std::exception e) {
            std::cout << "Exception detected in checkLoading(): " << e.what() << std::endl;
        }
    }

private:

    GLuint texture = 0;
    bool ispng = false;
    bool mLoaded = false;
    std::shared_ptr<IO::Image::Image> mImage = nullptr;
    unsigned char* image;

    std::string path;
    int m_TimesUsed = 0;
    double m_TimeSinceLastUse = 0.0;

    int width = 0, height = 0;

};

#endif /// TEXTURE_H_INCLUDED
