#pragma once

#include <Core.hpp>

namespace Epsilon
{
    namespace API
    {
        class TextureBase
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

                GLuint TextureId;
                GLuint Border;

                GLenum Type;
                GLenum Format;
                GLenum Target;
                GLenum Filtering;
                GLenum Wrapping;

                bool SRGB;
                bool Compressed;

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
            TextureBase()
            {
                mData.TextureId = 0;
            }

            TextureBase(TextureData) {}

            virtual ~TextureBase() {}

            virtual uint32_t ID() { return mData.TextureId; }

            virtual uint32_t Width() = 0;

            virtual uint32_t Height() = 0;

            virtual uint32_t NumFaces() = 0;

            virtual void Fill(uint8_t * /*data*/, size_t /*level*/, size_t /*offset*/) = 0;

            virtual void Bind() = 0;

            virtual void Bind(int) = 0;

            virtual void Unbind() = 0;

            virtual void Destroy() = 0;

        protected:
            TextureData mData;
        };
    } // namespace API
} // namespace Epsilon
