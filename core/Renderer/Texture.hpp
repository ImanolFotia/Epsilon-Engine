#pragma once

#include <pch.hpp>
#include <Core.hpp>

#include <Driver/API/TextureBase.hpp>
#include <Driver/API/OpenGL/Texture2D.hpp>
#include <Driver/API/OpenGL/Texture1D.hpp>
#include <Driver/API/OpenGL/Context.hpp>

#include <Driver/API/Context.hpp>

#include <Resource/Image/Image.hpp>

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

            virtual unsigned int ID() = 0;

        protected:
            std::shared_ptr<API::TextureBase> mTexture;
            API::ContextBase_ptr mContext;
        };
    } // namespace Renderer
} // namespace Epsilon
