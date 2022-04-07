#pragma once

#include <Core.hpp>

namespace Epsilon::API
{
    template <typename T>
    class Framebuffer
    {
    public:
        Framebuffer(int width, int height, bool DepthAttachment) {}

        Framebuffer() = default;

        Framebuffer(const Framebuffer &) = delete;
        Framebuffer(Framebuffer &&) = delete;

        ~Framebuffer() {}

        virtual bool addRenderTarget(T name, int internalformat, int format, int magfilter, int minfilter, bool mipmaps, GLuint target = GL_TEXTURE_2D, GLenum internal_type = GL_FLOAT, bool srgb = true) = 0;

        virtual bool FinishFrameBuffer(bool no_color_buffers = false) = 0;

        virtual bool addDepthAttachment() = 0;

        virtual bool addStencilAttachment() = 0;

        virtual void bindFramebuffer() = 0;

        virtual void unbindFramebuffer() = 0;

        virtual void AttachRenderBuffer(T index) = 0;

        virtual void AttachRenderBuffer(T index, int face, int mip = 0) = 0;

        virtual void BindDepthBuffer(int w, int h) = 0;

        virtual void BindDepthBuffer() = 0;

        virtual void ClearAttachment(T att, const void *col) = 0;

        virtual void setViewport() = 0;

        virtual void setViewport(int w, int h) = 0;

        virtual void clearBuffer(int masks) = 0;

        virtual void UpdateMipMaps() = 0;

        virtual void setToRead() = 0;

        virtual void setToDraw() = 0;

        virtual void Resize(uint32_t w, uint32_t h) = 0;

        virtual bool checkFramebuffer() = 0;

        virtual bool Destroy() = 0;

        virtual void MakeInmutable(T index) = 0;

        virtual GLuint getRenderTargetHandler(T name) = 0;

        virtual Epsilon::API::Texture_ptr getTexture(T name) = 0;

        virtual glm::vec2 Resolution() = 0;
    };
}