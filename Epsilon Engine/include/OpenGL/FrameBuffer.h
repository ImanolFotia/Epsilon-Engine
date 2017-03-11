#pragma once

#include <unordered_map>
#include <iostream>
#include <string>
#include <memory>
#include <GL/glew.h>
#include <OpenGL/RenderTarget.h>

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

    bool addRenderTarget(std::string name, int internalformat, int format, int magfilter, int minfilter, bool mipmaps)
    {
        if(m_RenderTargetCount >= MAX_RENDER_TARGETS)
        {
            std::cout << "FRAMEBUFFER ERROR: MAX DRAW BUFFERS REACHED" << std::endl;
            return false;
        }

        this->bindFramebuffer();

        m_RenderTargets[name] = (std::shared_ptr<RenderTarget>) new RenderTarget(WIDTH, HEIGHT, internalformat, format, magfilter, minfilter, GL_COLOR_ATTACHMENT0 + m_RenderTargetCount, mipmaps);

        m_RenderTargetCount++;

        this->unbindFramebuffer();
    }

    bool FinishFrameBuffer()
    {
        if(isDepthAttachment)
            addDepthAttachment();

        this->bindFramebuffer();

        GLenum DrawBuffers[m_RenderTargetCount];

        for(auto i = 0; i < m_RenderTargetCount; ++i)
            DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

        glDrawBuffers(m_RenderTargetCount, DrawBuffers);

        this->checkFramebuffer();
    }

    bool addDepthAttachment()
    {
        this->bindFramebuffer();
        glGenRenderbuffers(1, &m_DepthTextureTarget);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTextureTarget);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthTextureTarget);
        this->unbindFramebuffer();
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
    }

    bool Destroy()
    {
        glDeleteFramebuffers(1, &m_FramebufferHandler);
    }

    GLuint getRenderTargetHandler(std::string name)
    {
        return m_RenderTargets[name]->getRenderTextureTarget();
    }

    std::unordered_map<std::string, std::shared_ptr<RenderTarget> > m_RenderTargets;

    int m_RenderTargetCount;
    int MAX_RENDER_TARGETS;
    int WIDTH;
    int HEIGHT;

    GLuint m_FramebufferHandler;
    GLuint m_DepthTextureTarget;

    bool isDepthAttachment;
};


