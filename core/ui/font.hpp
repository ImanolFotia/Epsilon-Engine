#pragma once

#include <string>
#include <vector>

namespace UI {
struct AlignedQuad {
  float x0, y0, s0, t0; // top-left
  float x1, y1, s1, t1; // bottom-right
};

struct Font {

  const std::vector<unsigned char> &Pixels() { return pixels; }
  const std::vector<UI::AlignedQuad> &Glyphs() { return glyphs; }

  int Dimensions() { return dimensions; }
  int FontSize() { return font_size; }
  const std::string &Name() { return font_name; }

protected:
  std::string font_name{};
  int font_size{};
  int dimensions{};
  std::vector<unsigned char> pixels{};
  std::vector<UI::AlignedQuad> glyphs{};
};
} // namespace UI