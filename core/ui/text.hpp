#pragma once

#include "../engine/context.hpp"

namespace engine
{
    class TextRenderer
    {
        Ref<Texture> m_pFontTexture;

        TextRenderer() {}
        TextRenderer(const std::string &fontname)
        {
            LoadFont(std::forward(fontname));
        }

        void LoadFont(const std::string &fontname)
        {

            engine::Ref<engine::Material> textMaterial;
            {
                int w, h, nc;
                unsigned char *pixels = framework::load_image_from_file(fontname,
                                                                        &w,
                                                                        &h,
                                                                        &nc);
                engine::TextureCreationInfo texInfo = engine::TextureBuilder()
                                                          .width(w)
                                                          .height(h)
                                                          .numChannels(nc)
                                                          .pixels(pixels)
                                                          .name(fontname);

                textMaterial = Epsilon::getContext().ResourceManager()->createTexture(texInfo);
                framework::free_image_data(pixels);
            }
        }
    };
} // namespace engine
