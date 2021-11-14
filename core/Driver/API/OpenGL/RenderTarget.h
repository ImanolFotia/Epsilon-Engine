#pragma once
#include <Core.hpp>
#include "../Texture.hpp"
#include "Texture2D.hpp"

#include "HelperFunctions/CheckError.h"

class RenderTarget
{
public:
    unsigned int mWidth;
    unsigned int mHeight;
    GLuint mInternalFormat = 0;
    GLuint mFormat;
    GLuint mMagFilter;
    GLuint mMinFilter;
    bool mMipMaps;
    GLuint mTarget;
    GLuint mAttachment = GL_DEPTH_ATTACHMENT;
    GLuint m_RenderTextureTarget = 0;
    Epsilon::API::Texture_ptr mTexture;
    GLenum mInternalType;

public:
    RenderTarget(uint32_t width, uint32_t height, uint32_t internalformat, uint32_t format, uint32_t magfilter, uint32_t minfilter, uint32_t attachment, bool mipmaps, GLuint target, GLenum internal_type, bool srgb)
        : mWidth{width}, mHeight{height}, mInternalFormat{internalformat}, mFormat{format}, mMagFilter{magfilter}, mMinFilter{minfilter}, mMipMaps{mipmaps}, mTarget{target}, mInternalType{internal_type}
    {
        glGenTextures(1, &m_RenderTextureTarget);
        glBindTexture(target, m_RenderTextureTarget);
        mAttachment = attachment;
        if (internalformat == GL_DEPTH_COMPONENT)
            mAttachment = GL_DEPTH_ATTACHMENT;

        if (target == GL_TEXTURE_CUBE_MAP)
        {

            for (unsigned int i = 0; i < 6; ++i)
            {
                // note that we store each face with 16 bit floating point values
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat,
                             width, height, 0, format, mInternalType, nullptr);
            }
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }
        else if (target == GL_TEXTURE_2D)
        {
            glTexImage2D(target, 0, internalformat, width, height, 0, format, mInternalType, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, m_RenderTextureTarget, 0);
        }

        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magfilter);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minfilter);

        Epsilon::glCheckError();
        if (mMipMaps)
            glGenerateMipmap(target);
        glBindTexture(target, 0);

        auto errorCode = Epsilon::glCheckError();
        if (errorCode != GL_NO_ERROR)
        {
            std::cout << "Error on: "
                      << "Framebuffer texture: "
                      << "\n"
                      << "Id: " << m_RenderTextureTarget << "\n"
                      << "Internal Format: " << mInternalFormat << "\n"
                      << "Target: " << mTarget << "\n"
                      << "Internal Type: " << mInternalType << std::endl;
        }
        using namespace Epsilon::API;

        //Texture::TextureData textureData = {internalformat, 3, 0, m_RenderTextureTarget, GL_FLOAT, target, 1, GL_CLAMP_TO_EDGE, true, false};

        Texture::TextureData textureData(internalformat, 4, 0, m_RenderTextureTarget, internal_type, target, 1, GL_CLAMP_TO_EDGE, srgb, false);
        textureData.Format = format;

        //mTexture = std::make_shared<OpenGL::Texture2D>(textureData, true);
    }
    ~RenderTarget() {}

    bool DestroyRenderTarget()
    {
        glDeleteTextures(1, &m_RenderTextureTarget);
        return true;
    }

    GLuint getRenderTextureTarget()
    {
        return m_RenderTextureTarget;
    }

    void Attach()
    {
        glFramebufferTexture(GL_FRAMEBUFFER, mAttachment, m_RenderTextureTarget, 0);
    }

    void Attach(int face, int mip = 0)
    {
        //glFramebufferTexture(GL_FRAMEBUFFER, mAttachment, m_RenderTextureTarget, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, mAttachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_RenderTextureTarget, mip);
    }

    void Bind()
    {
        glBindTexture(mTarget, m_RenderTextureTarget);
    }

    void Bind(int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(mTarget, m_RenderTextureTarget);
    }

    void Resize(unsigned int w, unsigned int h)
    {
        mWidth = w;
        mHeight = h;

        //glDeleteTextures(1, &m_RenderTextureTarget);

        glGenTextures(1, &m_RenderTextureTarget);
        glBindTexture(mTarget, m_RenderTextureTarget);

        if (mInternalFormat == GL_DEPTH_COMPONENT)
            mAttachment = GL_DEPTH_ATTACHMENT;

        if (mTarget == GL_TEXTURE_CUBE_MAP)
        {

            for (unsigned int i = 0; i < 6; ++i)
            {
                // note that we store each face with 16 bit floating point values
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat,
                             mWidth, mHeight, 0, mFormat, mInternalType, nullptr);
            }
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }
        else if (mTarget == GL_TEXTURE_2D)
        {
            glTexImage2D(mTarget, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mInternalType, 0);
            glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(mTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glFramebufferTexture(GL_FRAMEBUFFER, mAttachment, m_RenderTextureTarget, 0);
        }
        glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, mMagFilter);
        glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, mMinFilter);

        if (mMipMaps)
            glGenerateMipmap(mTarget);

        glBindTexture(mTarget, 0);
    }

    void MakeInmutable()
    {
        glBindTexture(mTarget, m_RenderTextureTarget);
        Attach();
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindTexture(mTarget, 0);
    }

private:
};
