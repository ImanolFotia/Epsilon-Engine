#pragma once

#include <Core.hpp>

namespace Epsilon::API
{
    class Texture
    {
    public:
        class TextureData
        {
        public:
            uint8_t NumChannels;
            uint8_t AnisotropyLevel;

            uint32_t Width;
            uint32_t Height;
            uint32_t InternalFormat;

            GLuint TextureId = 0;
            GLuint64 ResidentHandle = 0;
            GLuint Border;

            GLenum Type;
            GLenum Format;
            GLenum Target;
            GLenum Filtering;
            GLenum Wrapping;

            bool SRGB;
            bool Compressed;

            TextureData() = default;
            
            TextureData(uint32_t inf, uint8_t nc, uint8_t an, GLuint id, GLenum type, GLenum target,GLuint border, GLenum wrp, bool srgb, bool comp) {
                InternalFormat = inf;
                NumChannels = nc;
                AnisotropyLevel = an;
                TextureId = id;
                Type = type;
                Target = target;
                Wrapping = wrp;
                SRGB = srgb;
                Compressed = comp;
                Border = border;
            }

            void MakeDefaultGL()
            {
                //Default texture parameters
                Wrapping = GL_REPEAT;
                Compressed = true;
                Filtering = 1;
                AnisotropyLevel = 16;
                SRGB = true;
                Type = GL_UNSIGNED_BYTE;
                Format = GL_RGBA;
                Border = 0;
            }
        };

    public:
        Texture()
        {
            mData.TextureId = 0;
        }

        Texture(TextureData) {}

        virtual ~Texture() {}

        virtual uint32_t ID() { return mData.TextureId; }
        
        virtual uint32_t isResident() { return mIsResident; }
        
        virtual uint32_t Handle() { return mData.ResidentHandle; }

        virtual uint32_t Width() = 0;

        virtual uint32_t Height() = 0;

        virtual uint32_t NumFaces() = 0;

        virtual void Fill(uint8_t * /*data*/, size_t /*level*/, size_t /*offset*/) = 0;

        virtual void Bind() = 0;

        virtual void Bind(int) = 0;

        virtual void Unbind() = 0;

        virtual void Destroy() = 0;
        
        virtual bool wasCreated() { return texture_was_created; }

    protected:
        TextureData mData;
        bool texture_was_created = false;
        bool mIsResident = false;
    };

    using Texture_ptr = std::shared_ptr<Texture>;
} // namespace Epsilon
