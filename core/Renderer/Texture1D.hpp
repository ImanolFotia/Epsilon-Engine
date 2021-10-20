#pragma once

#include <pch.hpp>

#include <IO/IO.hpp>
#include <Engine.hpp>
#include "Texture.hpp"

namespace Epsilon::Renderer
{

    class Texture1D : public Texture
    {
    public:
        Texture1D() {}
        Texture1D(const API::ContextBase_ptr context)
        {
            mContext = context;
        }

        ~Texture1D() {}

        void Create(API::TextureBase::TextureData data) override
        {

            using API::CONTEXT_TYPE;
            auto type = Engine::Get().ContextType(); //CONTEXT_TYPE::OGL;//mContext->getType();
            switch (type)
            {
            case CONTEXT_TYPE::OGL:
                mTexture = std::make_shared<API::OpenGL::Texture1D>(data);
                break;

            default:
                IO::PrintLine("Context type: ", type, " is not yet implemented...");
                break;
            }
        }

        void Create(int w, int h) override
        {

            using API::CONTEXT_TYPE;
            auto type = Engine::Get().ContextType();
            switch (type)
            {
            case CONTEXT_TYPE::OGL:
                mTexture = std::make_shared<API::OpenGL::Texture1D>(w);
                break;

            default:
                IO::PrintLine("Context type: ", type, " is not yet implemented...");
                break;
            }
        }

        void setData(uint8_t *data, size_t size) override
        {
            mTexture->Fill(data, 0, 0);
        }

        API::Texture::TextureData getData() override
        {
            return mTexture->getData();
        }

        void Bind() override
        {
            mTexture->Bind();
        }

        void Bind(int slot) override
        {
            mTexture->Bind(slot);
        }

        void Unbind() override
        {
            mTexture->Unbind();
        }

        void Destroy()
        {
            mTexture->Destroy();
        }

    private:
        void _Create() {}
    };
}; // namespace Epsilon