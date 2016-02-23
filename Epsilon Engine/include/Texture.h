///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <iostream>

#include <GL/glew.h>

#include <SOIL.h>

#include <ProgramData.h>

using namespace std;

class eTexture
{

public:
    eTexture(const char* TexName)
    {
        ProgramData DATA;
        int channels;
        path = ("materials/" + std::string(TexName)).c_str();
        //cout << path << endl;
        unsigned char* image = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);
        if(!image)
            return;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        string name = string(TexName);
        std::size_t found2 = name.find("_s");
        std::size_t found1 = name.find("_n");
        std::size_t found3 = name.find("specular");
        std::size_t found4 = name.find("normal");
        std::size_t found5 = name.find("ddn");
        if(found1 != std::string::npos || found2 != std::string::npos || found3 != std::string::npos || found4 != std::string::npos || found5 != std::string::npos)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, DATA.ANISOTROPY);
        if(!texture)
        {
            cout << "no se pudo crear" << TexName << endl;
            return;
        }
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    virtual ~eTexture() {
            delete path;
        }

public:

    GLuint getTextureID(void)
    {
        return texture;
    }

    int getWidth()
    {
        return width;
    }

    int getHeight()
    {
        return height;
    }

    const char* getPath()
    {
        return path;
    }

private:

    GLuint texture = 0;

    const char* path;

    int width = 0, height = 0;

};

#endif /// TEXTURE_H_INCLUDED
