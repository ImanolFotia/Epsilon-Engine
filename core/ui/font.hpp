#pragma once

#include <string>
#include <vector>

namespace UI
{
  struct AlignedQuad
  {
    float x0, y0, s0, t0; // top-left
    float x1, y1, s1, t1; // bottom-right
  };

  struct vec2
  {
    float x;
    float y;
  };

  struct Font
  {

    const std::vector<unsigned char> &Pixels() { return pixels; }
    const std::vector<UI::AlignedQuad> &Glyphs() { return glyphs; }

    int Dimensions() { return dimensions; }
    int FontSize() { return font_size; }
    const std::string &Name() { return font_name; }

    vec2 white_pixel{};
    uint32_t num_chars = 95;
    uint32_t first_char = 32;
    uint32_t texture_size = 128;
    uint32_t highest_character = 0;

  protected:
    std::string font_name{};
    int font_size{};
    int dimensions{};
    std::vector<unsigned char> pixels{};
    std::vector<UI::AlignedQuad> glyphs{};
  };
} // namespace UI