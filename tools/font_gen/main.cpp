#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// #define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../thirdparty/stb/stb_truetype.h"

struct font_pixel
{
  uint8_t r, g, b, a;
};

struct AlignedQuad
{
  float x0, y0, s0, t0; // top-left
  float x1, y1, s1, t1; // bottom-right
};

struct Options
{
  int texture_size = 256;
  int font_size = 20;
  int first_char = 32;
  int num_chars = 95;
  std::string output_name{};
  std::string font_name{};
  std::string font_name_no_ext{};
};

struct vec2
{
  vec2() = default;
  vec2(int a, int b) : x{(float)a}, y{(float)b} {}
  vec2(float a, float b) : x{a}, y{b} {}
  vec2(double a, double b) : x{(float)a}, y{(float)b} {}

  vec2 operator/(float v) { return {x / v, y / v}; }

  float x{};
  float y{};
};

void CleanUpName(std::string &input)
{
  for (auto &chr : input)
  {
    if (chr == '-' || chr == '+' || chr == '.')
    {
      chr = '_';
    }
  }
}

Options ParseArgs(int argc, char **argv)
{
  Options options{};
  if (argc < 2)
  {
    std::cerr << "No files provided" << std::endl;
    std::exit(255);
  }

  options.font_name = std::string(argv[1]);
  options.font_name =
      options.font_name.substr(options.font_name.find_last_of('/') + 1, options.font_name.size());
  options.font_name_no_ext = options.font_name.substr(0, options.font_name.find_last_of('.'));

  CleanUpName(options.font_name_no_ext);

  for (int i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "-d") == 0)
    {
      options.texture_size = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-o") == 0)
    {
      options.output_name = argv[i + 1];
      i++;
    }

    if (std::strcmp(argv[i], "-s") == 0)
    {
      options.font_size = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-f") == 0)
    {
      options.first_char = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-n") == 0)
    {
      options.num_chars = std::atoi(argv[i + 1]);
      i++;
    }

    if (std::strcmp(argv[i], "-h") == 0)
    {
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

std::string str_toupper(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                 // static_cast<int(*)(int)>(std::toupper)         // wrong
                 // [](int c){ return std::toupper(c); }           // wrong
                 // [](char c){ return std::toupper(c); }          // wrong
                 [](unsigned char c)
                 { return std::toupper(c); } // correct
  );
  return s;
}

int main(int argc, char **argv)
{

  Options options = ParseArgs(argc, argv);

  std::string code_points_path = options.font_name.substr(0, options.font_name.find_last_of('.')) + ".codepoints";
  std::ifstream codepoints_file(code_points_path);

  std::cout << code_points_path << std::endl;
  std::string current, prev;
  int counted_chars = 0;
  int first_char_codepoint = 0;
  if (codepoints_file.is_open())
  {
    std::string line;
    while (std::getline(codepoints_file, line))
    {
      if (counted_chars == 0)
      {
        auto first_space = line.find_first_of(' ');
        first_char_codepoint = std::strtol(line.substr(first_space + 1).c_str(), nullptr, 16);
      }
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
  if (codepoints_file.is_open())
  {
    std::cout << counted_chars << std::endl;
    cdata.resize(counted_chars);
    options.num_chars = counted_chars;
    options.first_char = first_char_codepoint;
  }
  else
  {
    cdata.resize(options.num_chars);
  }

  unsigned char *ttf_buffer = new unsigned char[1 << 20];
  unsigned char *temp_bitmap = new unsigned char[options.texture_size * options.texture_size];

  fread(ttf_buffer, 1, 1 << 20, fopen(argv[1], "rb"));

  stbtt_BakeFontBitmap(ttf_buffer, 0, options.font_size, temp_bitmap, options.texture_size,
                       options.texture_size, options.first_char, options.num_chars, cdata.data());

  if (codepoints_file.is_open())
  {
    options.first_char = 0;
  }

  std::vector<AlignedQuad> m_AlignedQuads;
  m_AlignedQuads.resize(options.num_chars);
  for (int i = 0; i < options.num_chars; i++)
  {
    char c = i;
    float x = 0, y = 0;
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(cdata.data(), options.texture_size, options.texture_size, c, &x, &y, &q, 1);

    m_AlignedQuads[i] = {q.x0, q.y0, q.s0, q.t0, q.x1, q.y1, q.s1, q.t1};
  }

  for (int i = 0; i < options.texture_size * options.texture_size; i++)
  {
    pixels[i] = {255, 255, 255, temp_bitmap[i]};

    if (temp_bitmap[i] == 255)
    {
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
  if (codepoints_file.is_open())
  {
    std::string line;
    while (std::getline(codepoints_file, line))
    {
      outHeader << "    "
                << "static const int ";
      auto first_space = line.find_first_of(' ');
      outHeader << "ICON_" << str_toupper(line.substr(0, first_space));
      outHeader << " = " << index << "; // 0x" << str_toupper(line.substr(first_space + 1)) << std::endl;
      index++;
    }
    outHeader << "\n";
  }

  outHeader << "    " << options.font_name_no_ext << "(){\n";

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
            << "pixels.reserve(" << options.texture_size << "*"
            << options.texture_size << ");\n";
  outHeader << "        "
            << "pixels = {\n";

  outHeader << "           ";
  for (int i = 0; i < options.texture_size * options.texture_size; i++)
  {

    outHeader << (int)pixels[i].a << ", ";

    if (i % 14 == 0 && i != 0)
    {

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

  for (int i = 0; i < options.num_chars; i++)
  {
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
  outHeader << "};\n";
  outHeader << "}\n";
  outHeader << "};\n";
}