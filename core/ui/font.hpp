#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <vector>

namespace UI {
struct AlignedQuad {
  float x0, y0, s0, t0; // top-left
  float x1, y1, s1, t1; // bottom-right
};

struct Glyph {
  AlignedQuad quad;
  std::string code;
};

struct vec2 {
  float x;
  float y;
};

struct Font {

  const std::vector<unsigned char> &Pixels() { return pixels; }
  const std::vector<Glyph> &Glyphs() { return glyphs; }

  int Dimensions() { return dimensions; }
  int FontSize() { return font_size; }
  const std::string &Name() { return font_name; }

  vec2 white_pixel = {0.0f, 0.0f};
  uint32_t num_chars = 95;
  uint32_t first_char = 32;
  uint32_t texture_size = 128;
  uint32_t highest_character = 0;

  uint32_t min_range = 0;
  uint32_t max_range = 0;

protected:
  std::string font_name{};
  int font_size{};
  int dimensions{};
  std::vector<unsigned char> pixels{};
  std::vector<Glyph> glyphs{};

public:
  std::vector<uint32_t> glyph_index{};
  static constexpr const int ASCIIHexToInt[] = {
      // ASCII
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  };

  static std::string EncodeCodepoint(std::string codepoint) {
    int byte_count = 0;
    unsigned int hex_str = std::strtol(codepoint.c_str(), NULL, 16);
    uint8_t code_points[5] = {0};
    uint8_t bytes[4] = {0};

    for (int i = 0; i < codepoint.size(); i++) {
      code_points[i] = ASCIIHexToInt[codepoint[codepoint.size() - 1 - i]];
    }

    bytes[0] = ((0b00000011 & code_points[1]) << 4) | ((0xff & code_points[0]) >> 0) | (1 << 7);
    bytes[1] = ((0xff & code_points[2]) << 2) | ((0x0f & code_points[1]) >> 2) | (1 << 7);
    bytes[2] = ((0x0f & code_points[4]) << 4) | ((0xff & code_points[3]) >> 0) | (1 << 7);
    bytes[3] = ((0xff & code_points[4]) >> 2);

    if (hex_str > 0xFFFF) {
      byte_count = 4;
      bytes[3] |= 0xf0;
    } else if (hex_str > 0x7FF) {
      byte_count = 3;
      bytes[2] |= 0xe0;
    } else if (hex_str > 0x7F) {
      byte_count = 2;
      bytes[1] |= 0xC0;
    } else {
      byte_count = 1;
      bytes[0] &= 0b01111111;
    }
    std::string output{};
    for (int i = 0; i < byte_count; i++) {
      output += "\\x" + std::format("{:02x}", bytes[byte_count - i - 1]);
    }
    return output;
  }

  static uint32_t DecodeCodepoint(const char *codepoint) {

    int byte_count = std::popcount((uint8_t)(codepoint[0] & 0xf0)) - 1;

    uint32_t value = 0b00001111 & codepoint[byte_count];
    value |= (((0b00000011 & codepoint[byte_count - 1]) << 2) | ((0b01110000 & codepoint[byte_count]) >> 4)) << 4;
    value |= byte_count < 1 ? 0 : (((0b00111100 & codepoint[byte_count - 1]) >> 2)) << 8;
    value |= byte_count < 2 ? 0 : ((0b00001111 & codepoint[byte_count - 2])) << 12;
    value |= byte_count < 3 ? 0 : (((0b00000011 & codepoint[byte_count - 3]) << 2) | ((0b01110000 & codepoint[byte_count - 2]) >> 4)) << 16;

    return value;
  }
};
} // namespace UI