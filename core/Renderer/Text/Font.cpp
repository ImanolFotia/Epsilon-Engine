#include <pch.hpp>

#include "Font.hpp"

#include "SDFs/Roboto.hpp"


namespace Epsilon
{
    Font::Font() noexcept
    {
        const std::string path_suffix("materials/gui/");
        mFontTexture = std::make_unique<Renderer::Texture2D>();
        mFontTexture->Create(SDF::Roboto::Width, SDF::Roboto::Height);
        int w, h, c;
        unsigned char *data = stbi_load((path_suffix + SDF::Roboto::TextureName).c_str(), &w, &h, &c, 4);
        mFontTexture->setData(data, 0);
        stbi_image_free(data);

        for (unsigned i = 0; i < SDF::Roboto::CharacterCount; i++)
        {
            mCharacters[SDF::Roboto::characters_Roboto[i].codePoint] = SDF::Roboto::characters_Roboto[i];
        }
    }

    Font::Font(const std::string &tex) noexcept
    {
        const std::string path_suffix("materials/gui/");
        mFontTexture = std::make_unique<Renderer::Texture2D>();
        mFontTexture->Create(SDF::Roboto::Width, SDF::Roboto::Height);
        int w, h, c;
        unsigned char *data = stbi_load((path_suffix + tex).c_str(), &w, &h, &c, 4);
        mFontTexture->setData(data, 0);
        stbi_image_free(data);
    }
} // namespace Epsilon