#pragma once
#include <GL/glew.h>

class RenderTarget
{
public:
    RenderTarget(int width, int height, int internalformat, int format, int magfilter, int minfilter, int attachment,bool mipmaps)
    {
        glGenTextures(1, &m_RenderTextureTarget);
        glBindTexture(GL_TEXTURE_2D, m_RenderTextureTarget);
        glTexImage2D(GL_TEXTURE_2D, 0,internalformat, width, height, 0,format, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_RenderTextureTarget, 0);
        if(mipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    ~RenderTarget() {}

    bool DestroyRenderTarget()
    {
        glDeleteTextures(1, &m_RenderTextureTarget);
    }

    GLuint getRenderTextureTarget()
    {
        return m_RenderTextureTarget;
    }

private:
    GLuint m_RenderTextureTarget;
};

