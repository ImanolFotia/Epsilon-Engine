#pragma once

#include "TextureBase.hpp"
namespace Epsilon
{
    namespace API
    {
        namespace OpenGL
        {
            class Texture1D : public TextureBase
            {
            public:
                Texture1D(uint32_t w)
                {
                    mData.Width = w;
                    mData.Height = 0;
                    mData.Target = GL_TEXTURE_1D;

                    _Create();
                }

                virtual ~Texture1D() noexcept {}

                virtual uint32_t Width() override
                {
                    return mData.Width;
                }

                virtual uint32_t Height() override
                {
                    return mData.Height;
                }

                virtual uint32_t NumFaces() override
                {
                    return 1;
                }

                void Fill(uint8_t *data, size_t level, size_t offset) override
                {

                    glBindTexture(mData.Target, mData.TextureId);
                    glTexSubImage1D(mData.Target, level, offset, mData.Width, mData.InternalFormat, mData.Type, data);
                    glBindTexture(mData.Target, 0);
                }

            private:
                void _Create()
                {
                    glGenTextures(1, &mData.TextureId);
                    glBindTexture(mData.Target, mData.TextureId);
                    _EvaluateInternalFormat();
                    _EvaluateFiltering();
                    glTexParameteri(mData.Target, GL_TEXTURE_WRAP_S, mData.Wrapping);
                    glTexParameteri(mData.Target, GL_TEXTURE_WRAP_T, mData.Wrapping);
                    glBindTexture(mData.Target, 0);
                }

                void _EvaluateInternalFormat()
                {
                    if (mData.SRGB)
                    {
                        if (mData.Compressed)
                        {
                            mData.InternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
                        }
                        else
                        {
                            mData.InternalFormat = GL_SRGB_ALPHA;
                        }
                    }
                    else
                    {
                        if (mData.Compressed)
                        {
                            mData.InternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                        }
                        else
                        {
                            mData.InternalFormat = GL_RGBA;
                        }
                    }
                    glTexImage1D(mData.Target, 0, mData.InternalFormat, mData.Width, mData.Height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                }

                void _EvaluateFiltering()
                {

                    if (mData.Filtering == 0 /*no filtering at all*/ && mData.AnisotropyLevel == 0)
                    {

                        glTexParameteri(mData.Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexParameteri(mData.Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    }
                    else if (mData.Filtering == 1 /* Linear filtering without mip mapping*/ && mData.AnisotropyLevel == 0)
                    {

                        glTexParameteri(mData.Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(mData.Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    }
                    else if (mData.Filtering == 2 /* Linear filtering with mip mapping*/ && mData.AnisotropyLevel == 0)
                    {
                        glTexParameteri(mData.Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(mData.Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glGenerateMipmap(mData.Target);
                    }
                    else /*Texture uses some level of anisotropic filtering*/
                    {
                        glTexParameterf(mData.Target, GL_TEXTURE_MAX_ANISOTROPY_EXT, mData.AnisotropyLevel);
                        glGenerateMipmap(mData.Target);
                    }
                }
            };
        } // namespace OpenGL
    }     // namespace API
} // namespace Epsilon