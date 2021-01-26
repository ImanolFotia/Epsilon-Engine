#pragma once

#include <pch.hpp>

#include <IO/IO.hpp>
#include <Engine.hpp>
#include "Texture.hpp"

namespace Epsilon
{
    namespace Renderer
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

            void Create(API::TextureBase::TextureData data) override
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
                mTexture->Fill(data, 0, 0);
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

            unsigned int ID() override {
                return mTexture->ID();
            }

        private:
            void _Create() {}
        };
    } // namespace Renderer
} // namespace Epsilon