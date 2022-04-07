#pragma once

#include <pch.hpp>
#include <glm/glm.hpp>
#include <Core.hpp>
#include <Driver/API/OpenGL/RenderTarget.h>
#include "Texture2D.hpp"

#include "../Framebuffer.hpp"

namespace Epsilon::OpenGL
{
    template <typename T>
    class FrameBuffer : public Epsilon::API::Framebuffer<T>
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

        FrameBuffer() //Default constructor, is used to refernce default framebuffer
        {
            mIsDefaultFramebuffer = true;
            m_FramebufferHandler = 0;
        }

        FrameBuffer(const FrameBuffer &) = delete;
        FrameBuffer(FrameBuffer &&) = delete;

        ~FrameBuffer() {}

        bool addRenderTarget(T name, int internalformat, int format, int magfilter, int minfilter, bool mipmaps, GLuint target = GL_TEXTURE_2D, GLenum internal_type = GL_FLOAT, bool srgb = true)
        {
            if (mIsDefaultFramebuffer)
                return false;

            if (m_RenderTargetCount >= MAX_RENDER_TARGETS)
            {
                std::cout << "FRAMEBUFFER ERROR: MAX DRAW BUFFERS REACHED" << std::endl;
                return false;
            }

            this->bindFramebuffer();

            m_RenderTargets[name] = (std::shared_ptr<RenderTarget>)new RenderTarget(WIDTH, HEIGHT, internalformat, format, magfilter, minfilter, GL_COLOR_ATTACHMENT0 + m_RenderTargetCount, mipmaps, target, internal_type, srgb);

            m_RenderTargetCount++;

            this->unbindFramebuffer();

            return true;
        }

        bool FinishFrameBuffer(bool no_color_buffers = false)
        {
            if (mIsDefaultFramebuffer)
                return false;

            if (isDepthAttachment)
                if(mDepthAttachmentType == GL_DEPTH_COMPONENT32F || mDepthAttachmentType == GL_DEPTH_COMPONENT16)
                    addDepthAttachment();
                else 
                    addStencilAttachment();

            this->bindFramebuffer();

            std::vector<GLenum> DrawBuffers(m_RenderTargetCount);

            if (!no_color_buffers)
            {
                for (auto i = 0; i < m_RenderTargetCount; ++i)
                    DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

                glDrawBuffers(m_RenderTargetCount, &DrawBuffers[0]);
            }

            this->checkFramebuffer();

            return true;
        }

        bool addDepthAttachment()
        {
            if (mIsDefaultFramebuffer)
                return false;
                
            mDepthAttachmentType = GL_DEPTH_COMPONENT32F;

            if(m_DepthTextureTarget != 0)
            glDeleteRenderbuffers(1, &m_DepthTextureTarget);

            this->bindFramebuffer();
            glGenRenderbuffers(1, &m_DepthTextureTarget);
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTextureTarget);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, WIDTH, HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthTextureTarget);
            this->unbindFramebuffer();

            return true;
        }

        bool addStencilAttachment() {
            if(mIsDefaultFramebuffer) return false;
            mDepthAttachmentType = GL_DEPTH32F_STENCIL8;

            if(m_DepthTextureTarget != 0)
            glDeleteRenderbuffers(1, &m_DepthTextureTarget);

            bindFramebuffer(); 
            glGenRenderbuffers(1, &m_DepthTextureTarget);
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTextureTarget);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, WIDTH, HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthTextureTarget);
            unbindFramebuffer();

            return true;
        }

        void bindFramebuffer()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandler);
        }

        void unbindFramebuffer() override
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void AttachRenderBuffer(T index)
        {
            if (mIsDefaultFramebuffer)
                return;
            m_RenderTargets[index]->Attach();
        }
        
        void AttachRenderBuffer(T index, int face, int mip = 0)
        {
            if (mIsDefaultFramebuffer)
                return;
            m_RenderTargets[index]->Attach(face, mip);
        }

        void BindDepthBuffer(int w, int h)
        {
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTextureTarget);
            glRenderbufferStorage(GL_RENDERBUFFER, mDepthAttachmentType, w, h);
        }
        
        void BindDepthBuffer()
        {
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTextureTarget);
            glRenderbufferStorage(GL_RENDERBUFFER, mDepthAttachmentType, WIDTH, HEIGHT);
        }

        void ClearAttachment(T att, const void *col)
        {
            if (mIsDefaultFramebuffer)
                return;

            auto target = m_RenderTargets.at(att);
            auto tex = target->getRenderTextureTarget();
            glClearTexImage(tex, 0, target->mInternalFormat, target->mInternalType, col);
        }

        void setViewport()
        {
            glViewport(0, 0, WIDTH, HEIGHT);
        }

        void setViewport(int w, int h)
        {
            glViewport(0, 0, w, h);
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

        void Resize(uint32_t w, uint32_t h)
        {

            if (mIsDefaultFramebuffer)
                return;
            WIDTH = w;
            HEIGHT = h;

            glDeleteFramebuffers(1, &m_FramebufferHandler);
            m_FramebufferHandler = 0;
            glGenFramebuffers(1, &m_FramebufferHandler);

            this->bindFramebuffer();

            for (const auto &[name, target] : m_RenderTargets)
            {
                target->Resize(WIDTH, HEIGHT);
            }

            if (isDepthAttachment)
            {
                glDeleteRenderbuffers(1, &m_DepthTextureTarget);/*
                glGenRenderbuffers(1, &m_DepthTextureTarget);
                glBindRenderbuffer(GL_RENDERBUFFER, m_DepthTextureTarget);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthTextureTarget);*/
            }

            FinishFrameBuffer();

            this->unbindFramebuffer();
        }

        bool checkFramebuffer()
        {

            if (mIsDefaultFramebuffer)
                return false;
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
            if (mIsDefaultFramebuffer)
                return false;
            glDeleteFramebuffers(1, &m_FramebufferHandler);

            return true;
        }

        void MakeInmutable(T index)
        {
            if (mIsDefaultFramebuffer)
                return;
            glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandler);
            m_RenderTargets.at(index)->MakeInmutable();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint getRenderTargetHandler(T name)
        {
            return m_RenderTargets.at(name)->getRenderTextureTarget();
        }

        Epsilon::API::Texture_ptr getTexture(T name)
        {
            return m_RenderTargets.at(name)->mTexture;
        }

        glm::vec2 Resolution()
        {
            return glm::vec2(WIDTH, HEIGHT);
        }

        std::unordered_map<T, std::shared_ptr<RenderTarget>> m_RenderTargets;

        int m_RenderTargetCount;
        int MAX_RENDER_TARGETS;
        int WIDTH;
        int HEIGHT;

        GLuint m_FramebufferHandler;
        GLuint m_DepthTextureTarget;

        bool isDepthAttachment;
        bool mIsDefaultFramebuffer = false;

        GLenum mDepthAttachmentType;
    };
} // namespace Epsilon
