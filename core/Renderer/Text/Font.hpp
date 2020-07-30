#pragma once

#include <pch.hpp>
#include <Texture.h>

#include "Character.hpp"

#include "SDFs/Roboto.hpp"

#include <SOIL.h>

namespace Epsilon
{
    class Font
    {
    public:
        Font() noexcept {
            const std::string path_suffix("materials/gui/");
            mFontTexture = std::make_unique<Renderer::Texture2D>();
            mFontTexture->Create(SDF::Roboto::Width, SDF::Roboto::Height);
            int w, h, c;
            unsigned char* data = SOIL_load_image((path_suffix + SDF::Roboto::TextureName).c_str(), &w, &h, &c, SOIL_LOAD_RGBA);
            mFontTexture->setData(data, 0);
            SOIL_free_image_data(data);

            for(unsigned i = 0; i < SDF::Roboto::CharacterCount; i++){
                mCharacters[SDF::Roboto::characters_Roboto[i].codePoint] = SDF::Roboto::characters_Roboto[i];
            }
        }

        Font(const std::string & tex) noexcept{
            const std::string path_suffix("materials/gui/");
            mFontTexture = std::make_unique<Renderer::Texture2D>();
            mFontTexture->Create(SDF::Roboto::Width, SDF::Roboto::Height);
            int w, h, c;
            unsigned char* data = SOIL_load_image((path_suffix + tex).c_str(), &w, &h, &c, SOIL_LOAD_RGBA);
            mFontTexture->setData(data, 0);
            SOIL_free_image_data(data);
        };

        const Character & Get(uint8_t c) {
            return mCharacters[c];
        }

        const std::string &Name()
        {
            return mName;
        }

        const uint32_t &Size()
        {
            return mSize;
        }

        const uint32_t &Width()
        {
            return mWidth;
        }

        const uint32_t &Height()
        {
            return mHeight;
        }

    private:
        std::string mName;
        uint32_t mSize = 0;
        uint32_t mBold = 0;
        uint32_t mItalic = 0;
        uint32_t mWidth = 0;
        uint32_t mHeight = 0;
        uint32_t mCharacterCount = 0;
        std::unordered_map<char, Character> mCharacters;
        std::unique_ptr<Renderer::Texture2D> mFontTexture;
    };
} // namespace Epsilon