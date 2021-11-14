#pragma once

#include <pch.hpp>

#include <IO/IO.hpp>
#include <Engine.hpp>
#include "Texture.hpp"

namespace Epsilon::Renderer
{

    class TextureCube : public Texture
    {
    public:
        TextureCube() {}
        TextureCube(const API::ContextBase_ptr context)
        {
            mContext = context;
        }

        ~TextureCube() {}

        void Create(API::Texture::TextureData data) override
        {

            using API::CONTEXT_TYPE;
            auto type = Engine::Get().ContextType(); //CONTEXT_TYPE::OGL;//mContext->getType();
            switch (type)
            {
            case CONTEXT_TYPE::OGL:
                mTexture = std::make_shared<API::OpenGL::TextureCube>(data);
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
                mTexture = std::make_shared<API::OpenGL::TextureCube>(w, h);
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
        
        unsigned int ID() override
        {
            if (!mTexture)
                return 0;
            return mTexture->ID();
        }

    private:
        void _Create() {}
    };
}; // namespace Epsilon