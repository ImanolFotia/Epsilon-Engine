#include <algorithm>
#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

// #define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../thirdparty/stb/stb_truetype.h"

struct font_pixel {
  uint8_t r, g, b, a;
};

struct AlignedQuad {
  float x0, y0, s0, t0; // top-left
  float x1, y1, s1, t1; // bottom-right
};

struct Glyph {
  AlignedQuad quad;
  std::string code = "\"\"";
};

struct Options {
  int texture_size = 256;
  int font_size = 20;
  int first_char = 32;
  int num_chars = 95;
  std::string output_name{};
  std::string font_name{};
  std::string font_name_no_ext{};
};

struct vec2 {
  vec2() = default;
  vec2(int a, int b) : x{(float)a}, y{(float)b} {}
  vec2(float a, float b) : x{a}, y{b} {}
  vec2(double a, double b) : x{(float)a}, y{(float)b} {}

  vec2 operator/(float v) { return {x / v, y / v}; }

  float x{};
  float y{};
};

void CleanUpName(std::string &input) {
  for (auto &chr : input) {
    if (chr == '-' || chr == '+' || chr == '.') {
      chr = '_';
    }
  }
}

Options ParseArgs(int argc, char **argv) {
  Options options{};
  if (argc < 2) {
    std::cerr << "No files provided" << std::endl;
    std::exit(255);
  }

  options.font_name = std::string(argv[1]);
  options.font_name = options.font_name.substr(options.font_name.find_last_of('/') + 1, options.font_name.size());
  options.font_name_no_ext = options.font_name.substr(0, options.font_name.find_last_of('.'));

  CleanUpName(options.font_name_no_ext);

  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "-d") == 0) {
      options.texture_size = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-o") == 0) {
      options.output_name = argv[i + 1];
      i++;
    }

    if (std::strcmp(argv[i], "-s") == 0) {
      options.font_size = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-f") == 0) {
      options.first_char = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-n") == 0) {
      options.num_chars = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-h") == 0) {
      std::cout << "Usage: font_gen file [options]\n";
      std::cout << "Options:\n";
      std::cout << "   -d\t\tImage/texture dimensions (default: 256x256)\n";
      std::cout << "   -o\t\tOutput file name (default: input file name)\n";
      std::cout << "   -s\t\tDesired font size (default: 20)\n";
      std::cout << "   -h\t\tPrint this message\n";
      std::cout << std::endl;
      std::exit(0);
    }
  }

  return options;
}

std::string str_toupper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 // static_cast<int(*)(int)>(std::toupper)         // wrong
                 // [](int c){ return std::toupper(c); }           // wrong
                 // [](char c){ return std::toupper(c); }          // wrong
                 [](unsigned char c) { return std::toupper(c); } // correct
  );
  return s;
}

int ASCIIHexToInt[] = {
    // ASCII
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

std::string EncodeCodepoint(std::string codepoint) {
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
  std::string output = "\"";
  for (int i = 0; i < byte_count; i++) {
    output += "\\x" + std::format("{:02x}", bytes[byte_count - i - 1]);
  }
  output += "\"";
  return output;
}

std::string DecodeCodepoint(const char *codepoint) {

  int byte_count = std::popcount((uint8_t)(codepoint[0] & 0xf0)) - 1;

  uint32_t value = 0b00001111 & codepoint[byte_count];
  value |= (((0b00000011 & codepoint[byte_count - 1]) << 2) | ((0b01110000 & codepoint[byte_count]) >> 4)) << 4;
  value |= byte_count < 1 ? 0 : (((0b00111100 & codepoint[byte_count - 1]) >> 2)) << 8;
  value |= byte_count < 2 ? 0 : ((0b00001111 & codepoint[byte_count - 2])) << 12;
  value |= byte_count < 3 ? 0 : (((0b00000011 & codepoint[byte_count - 3]) << 2) | ((0b01110000 & codepoint[byte_count - 2]) >> 4)) << 16;

  return std::format("U+{:04x}", value);
}

int main(int argc, char **argv) {

  Options options = ParseArgs(argc, argv);

  std::string code_points_path = options.font_name.substr(0, options.font_name.find_last_of('.')) + ".codepoints";
  std::ifstream codepoints_file(code_points_path);

  std::cout << code_points_path << std::endl;
  std::string current, prev;
  int counted_chars = 0;
  int first_char_codepoint = 0;
  std::vector<std::string> codepoints;
  uint32_t min = std::numeric_limits<uint32_t>::max();
  uint32_t max = 0;
  if (codepoints_file.is_open()) {
    std::string line;
    while (std::getline(codepoints_file, line)) {
      auto first_space = line.find_first_of(' ');
      if (counted_chars == 0) {
        first_char_codepoint = std::strtol(line.substr(first_space + 1).c_str(), nullptr, 16);
      }
      codepoints.push_back(line.substr(first_space + 1));
      uint32_t code = strtol(str_toupper(line.substr(first_space + 1)).c_str(), nullptr, 16);
      if (code > max)
        max = code;
      if (code < min)
        min = code;
      if (line == prev)
        continue;
      prev = line;
      counted_chars++;
    }
  }

  codepoints_file.seekg(0, std::ios::beg);
  vec2 m_WhitePixelPos{};
  std::vector<font_pixel> pixels{};
  pixels.resize(options.texture_size * options.texture_size);

  std::vector<stbtt_bakedchar> cdata;
  if (codepoints_file.is_open()) {
    std::cout << counted_chars << std::endl;
    cdata.resize(counted_chars);
    options.num_chars = counted_chars;
    options.first_char = first_char_codepoint;
  } else {
    cdata.resize(options.num_chars);
  }

  unsigned char *ttf_buffer = new unsigned char[1 << 20];
  unsigned char *temp_bitmap = new unsigned char[options.texture_size * options.texture_size];

  fread(ttf_buffer, 1, 1 << 20, fopen(argv[1], "rb"));

  stbtt_BakeFontBitmap(ttf_buffer, 0, options.font_size, temp_bitmap, options.texture_size, options.texture_size, options.first_char, options.num_chars, cdata.data());

  if (codepoints_file.is_open()) {
    options.first_char = 0;
  }

  std::vector<Glyph> m_AlignedQuads;
  m_AlignedQuads.resize(options.num_chars);
  for (int i = 0; i < options.num_chars; i++) {
    char c = i;
    float x = 0, y = 0;
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(cdata.data(), options.texture_size, options.texture_size, c, &x, &y, &q, 1);
    if (q.s0 > 1.0f || q.s1 > 1.0f || q.t0 > 1.0f || q.t1 > 1.0f)
      continue;

    std::string code = "\" \"";
    code[1] = (char)(i + options.first_char);
    if (i + options.first_char == 34) {
      code = "\"\\\"\"";
    } else if (i + options.first_char == 92) {

      code = "\"\\\\\"";
    }
    if (codepoints.size() > 0)
      if (min > 0x7F || i + options.first_char > 0x7F) {
        std::cout << "encoding" << std::endl;
        code = EncodeCodepoint(codepoints.at(i));
      }
    m_AlignedQuads[i] = {q.x0, q.y0, q.s0, q.t0, q.x1, q.y1, q.s1, q.t1, code};
  }

  for (int i = 0; i < options.texture_size * options.texture_size; i++) {
    pixels[i] = {255, 255, 255, temp_bitmap[i]};

    if (temp_bitmap[i] == 255) {
      m_WhitePixelPos = vec2(i % options.texture_size, i / options.texture_size) / 255.0f;
    }
  }

  delete[] temp_bitmap;
  delete[] ttf_buffer;
  codepoints_file.close();
  codepoints_file = std::ifstream(code_points_path);

  std::ofstream outHeader(options.font_name_no_ext + ".hpp");
  outHeader << "#pragma once\n\n#include <core/ui/font.hpp>\n\n";
  outHeader << "//" << options.font_name_no_ext << " font\n";
  outHeader << "struct " << options.font_name_no_ext << " : UI::Font{\n\n";

  int index = 0;
  if (codepoints_file.is_open()) {
    std::string line;
    while (std::getline(codepoints_file, line)) {
      outHeader << "    "
                << "static constexpr const char* ";
      auto first_space = line.find_first_of(' ');
      outHeader << "ICON_" << str_toupper(line.substr(0, first_space));
      outHeader << " = " << EncodeCodepoint(line.substr(first_space + 1)) << "; // U+" << str_toupper(line.substr(first_space + 1)) << std::endl;

      index++;
    }
    outHeader << "\n";
  }

  outHeader << "    " << options.font_name_no_ext << "(){\n";

  outHeader << "        "
            << "min_range = 0x" << std::format("{:04x}", min) << ";\n";
  outHeader << "        "
            << "max_range = 0x" << std::format("{:04x}", max) << ";\n";
  outHeader << "        "
            << "font_name = \"" << options.font_name << "\";\n";
  outHeader << "        "
            << "font_size = " << options.font_size << ";\n";
  outHeader << "        "
            << "dimensions = " << options.texture_size << ";\n";
  outHeader << "        "
            << "num_chars = " << (codepoints_file.is_open() ? index : options.num_chars) << ";\n";
  outHeader << "        "
            << "first_char = " << (codepoints_file.is_open() ? 0 : options.first_char) << ";\n";
  outHeader << "        "
            << "pixels.reserve(" << options.texture_size << "*" << options.texture_size << ");\n";
  outHeader << "        "
            << "pixels = {\n";

  outHeader << "           ";
  for (int i = 0; i < options.texture_size * options.texture_size; i++) {

    outHeader << (int)pixels[i].a << ", ";

    if (i % 14 == 0 && i != 0) {

      outHeader << "\n";
      outHeader << "           ";
    }
  }

  outHeader << "        "
            << "};\n\n";
  outHeader << "        "
            << "glyphs.reserve(" << options.num_chars << ");\n";
  outHeader << "        "
            << "glyphs = {\n";

  for (int i = 0; i < options.num_chars; i++) {
    if (i % 2 == 1 || i == 0)
      outHeader << "            ";
    outHeader << "{" << m_AlignedQuads[i].quad.x0 << ", " << m_AlignedQuads[i].quad.y0 << ", " << m_AlignedQuads[i].quad.s0 << ", " << m_AlignedQuads[i].quad.t0 << ",";
    outHeader << m_AlignedQuads[i].quad.x1 << ", " << m_AlignedQuads[i].quad.y1 << ", " << m_AlignedQuads[i].quad.s1 << ", " << m_AlignedQuads[i].quad.t1 << ", " << m_AlignedQuads[i].code << "},";
    if (i % 2 == 0)
      outHeader << "\n";
  }
  outHeader << "};\n";
  outHeader << "}\n";
  outHeader << "};\n";
}