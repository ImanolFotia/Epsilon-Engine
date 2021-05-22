#pragma once

#include <pch.hpp>
#include <Core.hpp>

#include <Driver/API/Texture.hpp>
#include <Driver/API/OpenGL/Texture2D.hpp>
#include <Driver/API/OpenGL/Texture1D.hpp>
#include <Driver/API/OpenGL/Context.hpp>

#include <Driver/API/Context.hpp>

#include <Resource/Image/Image.hpp>

namespace Epsilon::Renderer
    {
        class Texture
        {
        public:
            Texture() {}
            Texture(const API::ContextBase_ptr) {}

            virtual ~Texture() {}

            virtual void Create(API::Texture::TextureData) = 0;

            virtual void Create(int, int) = 0;

            virtual void setData(uint8_t *, size_t) = 0;

            virtual void Bind() = 0;

            virtual void Bind(int) = 0;

            virtual void Unbind() = 0;

            virtual unsigned int ID() = 0;

            uint64_t Handle() { return mTexture->Handle(); }
            
            bool isResident() { return mTexture->isResident(); }

        protected:
            std::shared_ptr<API::Texture> mTexture;
            API::ContextBase_ptr mContext;
        };
} // namespace Epsilon
