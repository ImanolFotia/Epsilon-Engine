#pragma once

#include <pch.hpp>
#include <Core.hpp>
#include <Driver/API/OpenGL/RenderTarget.h>

template <typename T>
class FrameBuffer
{
public:
    FrameBuffer(int width, int height, bool DepthAttachment)
    {
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &MAX_RENDER_TARGETS);
        glGenFramebuffers(1, &m_FramebufferHandler);
        m_RenderTargetCount = 0;

        WIDTH = width;
        HEIGHT = height;

        isDepthAttachment = DepthAttachment;
    }

    ~FrameBuffer() {}

    bool addRenderTarget(T name, int internalformat, int format, int magfilter, int minfilter, bool mipmaps, GLuint target = GL_TEXTURE_2D)
    {
        if(m_RenderTargetCount >= MAX_RENDER_TARGETS)
        {
            std::cout << "FRAMEBUFFER ERROR: MAX DRAW BUFFERS REACHED" << std::endl;
            return false;
        }

        this->bindFramebuffer();

        m_RenderTargets[name] = (std::shared_ptr<RenderTarget>) new RenderTarget(WIDTH, HEIGHT, internalformat, format, magfilter, minfilter, GL_COLOR_ATTACHMENT0 + m_RenderTargetCount, mipmaps, target);

        m_RenderTargetCount++;

        this->unbindFramebuffer();

        return true;
    }

    bool FinishFrameBuffer()
    {
        if(isDepthAttachment)
            addDepthAttachment();

        this->bindFramebuffer();

        std::vector<GLenum> DrawBuffers(m_RenderTargetCount);

        for(auto i = 0; i < m_RenderTargetCount; ++i)
            DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

        glDrawBuffers(m_RenderTargetCount, &DrawBuffers[0]);

        this->checkFramebuffer();

        return true;
    }

    bool addDepthAttachment()
    {
        this->bindFramebuffer();
        glGenRenderbuffers(1, &m_DepthTextureTarget);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTextureTarget);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthTextureTarget);
        this->unbindFramebuffer();

        return true;
    }

    void bindFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandler);
    }

    void unbindFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void setViewport()
    {
        glViewport(0,0,WIDTH, HEIGHT);
    }
    
    void setViewport(int w, int h)
    {
        glViewport(0,0,w, h);
    }

    void clearBuffer(int masks)
    {
        glClear(masks);
    }

    void UpdateMipMaps()
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void setToRead()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferHandler);
    }

    void setToDraw()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferHandler);
    }

    bool checkFramebuffer()
    {
        this->bindFramebuffer();
        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (Status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Framebuffer error, status: " << Status << std::endl;
            return false;
        }
        this->unbindFramebuffer();

        return true;
    }

    bool Destroy()
    {
        glDeleteFramebuffers(1, &m_FramebufferHandler);

        return true;
    }

    GLuint getRenderTargetHandler(T name)
    {
        return m_RenderTargets[name]->getRenderTextureTarget();
    }

    std::unordered_map<T, std::shared_ptr<RenderTarget> > m_RenderTargets;

    int m_RenderTargetCount;
    int MAX_RENDER_TARGETS;
    int WIDTH;
    int HEIGHT;

    GLuint m_FramebufferHandler;
    GLuint m_DepthTextureTarget;

    bool isDepthAttachment;
};


