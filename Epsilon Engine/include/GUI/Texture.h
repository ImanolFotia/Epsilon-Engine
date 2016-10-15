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

#include <SOIL.h>


using namespace std;

class eTexture
{

public:
    eTexture(const char* TexName, GLenum wrap = GL_REPEAT)
    {
        int channels;
        path = (std::string(TexName)).c_str();
        unsigned char* image = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);
        std::cout << channels << std::endl;
        if(!image)
            return;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        if(!texture)
        {
            cout << "no se pudo crear" << TexName << endl;
            return;
        }
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    eTexture(std::vector<std::string> CubeMapPath)
    {
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        int channels;
        unsigned char* image;

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        for(GLuint i = 0; i < CubeMapPath.size(); i++)
        {
            image = SOIL_load_image(CubeMapPath[i].c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
                std::cout << CubeMapPath[i] << std::endl;
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
                std::cout << "CubeMapID" << texture << std::endl;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    virtual ~eTexture()
    {
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
