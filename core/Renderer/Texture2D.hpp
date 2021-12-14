#pragma once

#include <pch.hpp>

#include <IO/IO.hpp>
#include <Engine.hpp>
#include "Texture.hpp"

namespace Epsilon::Renderer
{

    class Texture2D : public Texture
    {
    public:
        Texture2D() {}
        Texture2D(const API::ContextBase_ptr context)
        {
            mContext = context;
        }

        ~Texture2D() {}

        void Create(API::Texture::TextureData data) override
        {
            using API::CONTEXT_TYPE;
            auto type = Engine::Get().ContextType(); //CONTEXT_TYPE::OGL;//mContext->getType();
            switch (type)
            {
            case CONTEXT_TYPE::OGL:
                mTexture = std::make_shared<API::OpenGL::Texture2D>(data);
                break;

            default:
                IO::PrintLine("Context type: ", type, " is not yet implemented...");
                break;
            }
        }

        void Create(int w, int h) override
        {

            using API::CONTEXT_TYPE;
            auto type = Engine::Get().ContextType(); //CONTEXT_TYPE::OGL;//mContext->getType();
            switch (type)
            {
            case CONTEXT_TYPE::OGL:
                mTexture = std::make_shared<API::OpenGL::Texture2D>(w, h);
                break;

            default:
                IO::PrintLine("Context type: ", type, " is not yet implemented...");
                break;
            }
        }

        void setData(uint8_t *data, size_t size) override
        {
            if (!mTexture)
                return;
            mTexture->Fill(data, 0, 0);
        }

        API::Texture::TextureData getData() override
        {
            return mTexture->getData();
        }

        void Bind() override
        {
            if (!mTexture)
                return;
            mTexture->Bind();
        }

        void Bind(int slot) override
        {
            if (!mTexture)
                return;
            mTexture->Bind(slot);
        }

        void Unbind() override
        {
            if (!mTexture)
                return;
            mTexture->Unbind();
        }

        void Destroy() override
        {
            if (!mTexture)
                return;
            mTexture->Destroy();
        }

        unsigned int ID() override
        {
            if (!mTexture)
                return 0;
            return mTexture->ID();
        }

        bool wasCreated()
        {
            if (mTexture == nullptr)
                return false;
            return mTexture->wasCreated();
        }

    private:
        void _Create() {}
    };
} // namespace Epsilon