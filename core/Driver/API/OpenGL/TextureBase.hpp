
#include <Core.hpp>

#pragma once

namespace Epsilon
{
    namespace API
    {
        namespace OpenGL
        {
            //Base abstract class for OpenGL texture
            class TextureBase
            {

                class TextureData
                {
                public:
                    uint8_t InternalFormat;
                    uint8_t NumChannels;
                    uint8_t AnisotropyLevel;

                    uint32_t Width;
                    uint32_t Height;

                    GLuint TextureId;

                    GLenum Type;
                    GLenum Target;
                    GLenum Filtering;
                    GLenum Wrapping;

                    bool SRGB;
                    bool Compressed;
                };

            public:
                TextureBase()
                {
                    mData.TextureId = 0;
                }
                virtual ~TextureBase() {}

                virtual uint32_t ID() { return mData.TextureId; }

                virtual uint32_t Width() = 0;

                virtual uint32_t Height() = 0;

                virtual uint32_t NumFaces() = 0;

                virtual void Fill(uint8_t * /*data*/, size_t /*level*/, size_t /*offset*/) = 0;

            protected:
                TextureData mData;
            };
        } // namespace OpenGL
    }     // namespace API
} // namespace Epsilon