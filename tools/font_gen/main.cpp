#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
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

struct vec2 {
  vec2() = default;
  vec2(int a, int b) : x{(float)a}, y{(float)b} {}
  vec2(float a, float b) : x{a}, y{b} {}
  vec2(double a, double b) : x{(float)a}, y{(float)b} {}

  vec2 operator/(float v) { return {x / v, y / v}; }

  float x{};
  float y{};
};

int texture_size = 256;
int font_size = 20;
std::string output_name{};
std::string font_name{};
std::string font_name_no_ext{};

void CleanUpName(std::string &input) {
  for (auto &chr : input) {
    if (chr == '-' || chr == '+' || chr == '.') {
      chr = '_';
    }
  }
}

void ParseArgs(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << "No files provided" << std::endl;
    return std::exit(255);
  }

  font_name = std::string(argv[1]);
  font_name =
      font_name.substr(font_name.find_last_of('/') + 1, font_name.size());
  font_name_no_ext = font_name.substr(0, font_name.find_last_of('.'));

  CleanUpName(font_name_no_ext);

  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "-d") == 0) {
      texture_size = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-o") == 0) {
      output_name = argv[i + 1];
      i++;
    }

    if (std::strcmp(argv[i], "-s") == 0) {
      font_size = std::atoi(argv[i + 1]);
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
      return std::exit(0);
    }
  }
}

int main(int argc, char **argv) {

  ParseArgs(argc, argv);

  vec2 m_WhitePixelPos{};
  std::vector<font_pixel> pixels{};
  pixels.resize(texture_size * texture_size);
  stbtt_bakedchar cdata[95];
  unsigned char *ttf_buffer = new unsigned char[1 << 20];
  unsigned char *temp_bitmap = new unsigned char[texture_size * texture_size];
  fread(ttf_buffer, 1, 1 << 20, fopen(argv[1], "rb"));
  stbtt_BakeFontBitmap(ttf_buffer, 0, font_size, temp_bitmap, texture_size,
                       texture_size, 32, 95, cdata);

  std::vector<AlignedQuad> m_AlignedQuads;
  m_AlignedQuads.resize(95);
  for (int i = 0; i < 95; i++) {
    char c = i;
    float x = 0, y = 0;
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(cdata, texture_size, texture_size, c, &x, &y, &q, 1);

    m_AlignedQuads[i] = {q.x0, q.y0, q.s0, q.t0, q.x1, q.y1, q.s1, q.t1};
  }

  for (int i = 0; i < texture_size * texture_size; i++) {
    pixels[i] = {255, 255, 255, temp_bitmap[i]};

    if (temp_bitmap[i] == 255) {
      m_WhitePixelPos = vec2(i % texture_size, i / texture_size) / 255.0f;
    }
  }

  delete[] temp_bitmap;
  delete[] ttf_buffer;

  std::ofstream outHeader(font_name_no_ext + ".hpp");
  outHeader << "#pragma once\n\n#include <core/ui/font.hpp>\n\n";
  outHeader << "//" << font_name_no_ext << " font\n";
  outHeader << "struct " << font_name_no_ext << " : UI::Font{\n\n";
  outHeader << "    " << font_name_no_ext << "(){\n";

  outHeader << "        " << "font_name = \"" << font_name << "\";\n";
  outHeader << "        " << "font_size = " << font_size << ";\n";
  outHeader << "        " << "dimensions = " << texture_size << ";\n";
  outHeader << "        " << "pixels.reserve(" << texture_size << "*"
            << texture_size << ");\n";
  outHeader << "        " << "pixels = {\n";

  outHeader << "           ";
  for (int i = 0; i < texture_size * texture_size; i++) {

    outHeader << (int)pixels[i].a << ", ";

    if (i % 14 == 0 && i != 0) {

      outHeader << "\n";
      outHeader << "           ";
    }
  }

  outHeader << "        " << "};\n\n";
  outHeader << "        " << "glyphs.reserve(" << 95 << ");\n";
  outHeader << "        " << "glyphs = {\n";

  for (int i = 0; i < 95; i++) {
    if (i % 2 == 1 || i == 0)
      outHeader << "            ";
    outHeader << "{" << m_AlignedQuads[i].x0 << ", " << m_AlignedQuads[i].y0
              << ", " << m_AlignedQuads[i].s0 << ", " << m_AlignedQuads[i].t0
              << ",";
    outHeader << m_AlignedQuads[i].x1 << ", " << m_AlignedQuads[i].y1 << ", "
              << m_AlignedQuads[i].s1 << ", " << m_AlignedQuads[i].t1 << "},";
    if (i % 2 == 0)
      outHeader << "\n";
  }
  outHeader << "<< "
               "};\n";
  outHeader << "<< "
               "}\n";
  outHeader << "};\n";
}