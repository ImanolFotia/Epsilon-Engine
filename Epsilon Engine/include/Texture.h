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

#include <ProgramData.h>

using namespace std;

class eTexture
{

public:
    eTexture(const char* TexName, GLenum wrap = GL_REPEAT, GLenum type = GL_TEXTURE_2D)
    {
        ProgramData DATA;
        int channels;
        path = ("materials/" + std::string(TexName)).c_str();
        //cout << path << endl;

        unsigned char* image = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);
        //cout << channels << endl;
        if(!image)
            return;
        glGenTextures(1, &texture);
        glBindTexture(type, texture);
        string name = string(TexName);
        std::size_t found2 = name.find("_s");
        std::size_t found1 = name.find("_n");
        std::size_t found3 = name.find("specular");
        std::size_t found4 = name.find("normal");
        std::size_t found5 = name.find("ddn");
        std::size_t found6 = name.find("nrm");
        if(found1 != std::string::npos || found2 != std::string::npos || found3 != std::string::npos || found4 != std::string::npos || found5 != std::string::npos || found6 != std::string::npos){
            if(type == GL_TEXTURE_2D)//GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
                glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            else if(type == GL_TEXTURE_1D)
                glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        else{
            if(type == GL_TEXTURE_2D)//GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
                glTexImage2D(type, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            else if(type == GL_TEXTURE_1D)
                glTexImage1D(type, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        glGenerateMipmap(type);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
        if(DATA.ANISOTROPY <= 0)
        {
            glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameterf(type, GL_TEXTURE_MAX_ANISOTROPY_EXT, DATA.ANISOTROPY);
        }
        if(!texture)
        {
            cout << "no se pudo crear" << TexName << endl;
            return;
        }
        SOIL_free_image_data(image);
        glBindTexture(type, 0);
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
