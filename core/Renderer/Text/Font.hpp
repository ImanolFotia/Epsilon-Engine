#pragma once

#include <pch.hpp>
#include <Texture.h>

#include "Character.hpp"

namespace Epsilon
{
    class Font
    {
    public:
        Font() noexcept;

        Font(const std::string & tex) noexcept;

        const Character & Get(uint8_t c) {
            return mCharacters[c];
        }

        const std::string &Name() { return mName; }

        const uint32_t &Size() { return mSize; }

        const uint32_t &Width() { return mWidth; }

        const uint32_t &Height() { return mHeight; }

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