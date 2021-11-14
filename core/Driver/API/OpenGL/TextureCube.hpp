#pragma once

#include "../Texture.hpp"

namespace Epsilon::API::OpenGL
{

    class TextureCube : public Texture
    {
    public:
        TextureCube(int w, int h)
        {
            mData.Width = w;
            mData.Height = h;
            mData.Target = GL_TEXTURE_CUBE_MAP;
            mData.MakeDefaultGL();

            _Create();
            texture_was_created = true;
        }

        TextureCube(TextureData data, bool was_created = false)
        {
            mData = data;
            mData.Target = GL_TEXTURE_CUBE_MAP;

            if (!was_created)
                _Create();
            texture_was_created = true;
        }

        ~TextureCube() {}

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
            return 6;
        }

        void Fill(uint8_t *data, size_t level = 0, size_t offset = 0) override
        {
            glBindTexture(mData.Target, mData.TextureId);

            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, mData.InternalFormat, mData.Width, mData.Height, 0, mData.Format, mData.Type, data);
            }
            glGenerateMipmap(mData.Target);
            glBindTexture(mData.Target, 0);
        }

        void Destroy() override
        {
            glDeleteTextures(1, &mData.TextureId);
        }

        void Bind() override
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, mData.TextureId);
        }

        void Bind(int slot) override
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mData.TextureId);
        }

        void Unbind() override
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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
            glTexParameteri(mData.Target, GL_TEXTURE_WRAP_R, mData.Wrapping);
            glBindTexture(mData.Target, 0);
        }

        void _EvaluateInternalFormat()
        {
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mData.InternalFormat, mData.Width, mData.Height, 0, mData.Format, mData.Type, nullptr);
            }

            glGenerateMipmap(mData.Target);
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
                glTexParameteri(mData.Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(mData.Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameterf(mData.Target, GL_TEXTURE_MAX_ANISOTROPY_EXT, mData.AnisotropyLevel);
                glGenerateMipmap(mData.Target);
            }
        }
    };
} // namespace Epsilon