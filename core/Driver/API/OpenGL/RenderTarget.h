#pragma once
#include <Core.hpp>

class RenderTarget {
public:
    RenderTarget(int width, int height, int internalformat, int format, int magfilter, int minfilter, int attachment,bool mipmaps, GLuint target) {

        glGenTextures(1, &m_RenderTextureTarget);
        glBindTexture(target, m_RenderTextureTarget);
        if(target == GL_TEXTURE_CUBE_MAP) {
            for (unsigned int i = 0; i < 6; ++i) {
                // note that we store each face with 16 bit floating point values
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                             width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            }

        } else if(target == GL_TEXTURE_2D) {
            glTexImage2D(target, 0,internalformat, width, height, 0,format, GL_FLOAT, 0);
        }
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magfilter);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minfilter);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        if(mipmaps)
            glGenerateMipmap(target);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, m_RenderTextureTarget, 0);
        glBindTexture(target, 0);
    }
    ~RenderTarget() {}

    bool DestroyRenderTarget() {
        glDeleteTextures(1, &m_RenderTextureTarget);
        return true;
    }

    GLuint getRenderTextureTarget() {
        return m_RenderTextureTarget;
    }

private:
    GLuint m_RenderTextureTarget;
};

