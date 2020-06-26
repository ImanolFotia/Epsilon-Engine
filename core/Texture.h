///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <Core.hpp>

#include <Engine.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <mutex>
#include <memory>



#include <Driver/API/Context.hpp>
#include <Driver/API/OpenGL/Texture2D.hpp>

#include <Image/Image.hpp>
#include <ProgramData.h>
#include <Helpers.hpp>
#include <Image/Image.hpp>
#include <Log.h>
#include <IO/IO.hpp>



namespace Epsilon
{
    namespace Renderer
    {
        class Texture
        {
        public:
            Texture() {}
            Texture(const API::ContextBase_ptr) {}

            virtual ~Texture() {}

            virtual void Create(API::TextureBase::TextureData) = 0;

            virtual void Create(int, int) = 0;

            virtual void setData(uint8_t *, size_t) = 0;

            virtual void Bind() = 0;

            virtual void Bind(int) = 0;

            virtual void Unbind() = 0;

        protected:
            std::shared_ptr<API::TextureBase> mTexture;
            API::ContextBase_ptr mContext;
        };

        class Texture2D : public Texture
        {
        public:
        
            Texture2D() {}
            Texture2D(const API::ContextBase_ptr context) {
                mContext = context;
            }

            ~Texture2D() {}

            void Create(API::TextureBase::TextureData data) override {
                
                using API::CONTEXT_TYPE;
                auto type = Engine::Get().ContextType();//CONTEXT_TYPE::OGL;//mContext->getType();
                switch (type)
                {
                case CONTEXT_TYPE::OGL:
                    mTexture = std::make_shared<API::OpenGL::Texture2D>(data);
                    break;
                
                default:
                    IO::PrintLine("Context type: ", type, " is not yet implemented...");
                    break;
                }
            }

            
            void Create(int w, int h) override {
                
                using API::CONTEXT_TYPE;
                auto type = Engine::Get().ContextType();//CONTEXT_TYPE::OGL;//mContext->getType();
                switch (type)
                {
                case CONTEXT_TYPE::OGL:
                    mTexture = std::make_shared<API::OpenGL::Texture2D>(w, h);
                    break;
                
                default:
                    IO::PrintLine("Context type: ", type, " is not yet implemented...");
                    break;
                }
            }

            void setData(uint8_t * data, size_t size) override {
                mTexture->Fill(data, 0, 0);
            }

            void Bind() override {
                mTexture->Bind();
            }
            
            void Bind(int slot) override {
                mTexture->Bind(slot);
            }

            void Unbind() override {
                mTexture->Unbind();
            }

            void Destroy() {
                mTexture->Destroy();
            }

            private:
                void _Create(){}
        };
    } // namespace Renderer
    
class eTexture {

public:
    eTexture(const char* TexName, GLenum wrap = GL_REPEAT, GLenum type = GL_TEXTURE_2D, GLuint filtering = -1) : mType(type) {
        ProgramData DATA;
        int channels;
        std::string ext = Helpers::getExtension(TexName);
        path = ("materials/" + std::string(TexName)).c_str();
        std::cout << "Loading Texture: " << path << std::endl;
        loadFile(path.c_str(), image, &width, &height, &channels);
        createGLTexture(image, TexName, wrap, type, filtering);
    }

    eTexture(std::vector<std::string> CubeMapPath) {
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        int channels;
        unsigned char* image;
        mType = GL_TEXTURE_CUBE_MAP;

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

    
    void Destroy() {
        glDeleteTextures(1, &texture);
    }

private:

    void createNullTexture(const char* TexName, GLenum wrap = GL_REPEAT, GLenum type = GL_TEXTURE_2D, GLuint filtering = -1)
    {
        //std::cout << "createNullTexture " << path << std::endl;
        ProgramData DATA;
        glGenTextures(1, &texture);
        glBindTexture(type, texture);
        
        bool isNonColorData = isNormal(TexName);

        internalFormat = isNonColorData ? GL_RGBA : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        internalFormat = DATA.COMPRESSED_TEXTURES ? internalFormat : GL_SRGB_ALPHA;
        
        if(mType == GL_TEXTURE_2D)
            CreateTexture2D();
        else if(type == GL_TEXTURE_1D)
            CreateTexture1D();

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
        
        glGenerateMipmap(type);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);

        if(!texture) {
            IO::PrintLine ("no se pudo crear", TexName);
            Log::WriteToLog("Texture: " + std::string(TexName) + " failed to load.");
            return;
        }
        glBindTexture(type, 0);
    }

    void CreateTexture3D()  = delete; //TODO

    void CreateTexture2D() {
        glTexImage2D(mType, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }

    void CreateTexture1D() {
        glTexImage1D(mType, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }

    void createGLTexture(unsigned char* image, const char* TexName, GLenum wrap = GL_REPEAT, GLenum type = GL_TEXTURE_2D, GLuint filtering = -1) {

        ProgramData DATA;
        if(!image)
            return;
            
        glGenTextures(1, &texture);
        bind();

        bool isNonColorData = isNormal(TexName);

        internalFormat = isNonColorData ? GL_RGBA : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        internalFormat = DATA.COMPRESSED_TEXTURES ? internalFormat : GL_SRGB_ALPHA;
        
        if(mType == GL_TEXTURE_2D)
            CreateTexture2D();
        else if(type == GL_TEXTURE_1D)
            CreateTexture1D();


        if(filtering == -1) {
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

        glGenerateMipmap(type);
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);

        if(!texture) {
            IO::PrintLine("no se pudo crear", TexName);
            Log::WriteToLog("Texture: " + std::string(TexName) + " failed to load.");
            return;
        }

        SOIL_free_image_data(image);
        //delete image;
        glBindTexture(type, 0);

        Log::WriteToLog("Texture: " + std::string(TexName) + " Loaded.");
        //mtx.unlock();
    }

    void loadFile(const char* path, unsigned char* &data, int *outwidth, int *outheight, int *outchannels) {
        data = SOIL_load_image(path, outwidth, outheight, outchannels, SOIL_LOAD_RGBA);
        Log::WriteToLog("Texture: " + std::string(path) + "Data Loaded.");
    }

    bool isNormal(const char* TexName) {
        std::string tex = std::string(TexName); 
        std::string name = Helpers::to_lower(tex);
        std::size_t found2 = name.find("_s");
        std::size_t found1 = name.find("_n");
        std::size_t found3 = name.find("specular");
        std::size_t found4 = name.find("normal");
        std::size_t found5 = name.find("ddn");
        std::size_t found6 = name.find("nrm");
        std::size_t found7 = name.find("Normal");
        std::size_t found8 = name.find("_m");
        std::size_t found9 = name.find("roughness");
        std::size_t found10 = name.find("metallic"); 
        std::size_t found11 = name.find("_metalness");
        std::size_t found12 = name.find("_gloss");
        std::size_t found13 = name.find("_r");

        std::size_t npos = std::string::npos;

        return          found1 != npos ||
                        found2 != npos ||
                        found3 != npos ||
                        found4 != npos ||
                        found5 != npos ||
                        found6 != npos ||
                        found7 != npos ||
                        found8 != npos ||
                        found9 != npos ||
                        found10 != npos ||
                        found11 != npos ||
                        found12 != npos ||
                        found13 != npos;
    }



public:

    virtual ~eTexture() {

    }
    GLuint getTextureID(void) {
        setRequestCount();
        //checkLoading();
        return texture;
    }

    void bind() {
        glBindTexture(mType, texture);
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

    GLenum mType = 0;
    uint32_t internalFormat = 0;

};


} // namespace Epsilon
#endif /// TEXTURE_H_INCLUDED