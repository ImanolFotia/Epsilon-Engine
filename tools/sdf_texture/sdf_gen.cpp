#include <algorithm>
#include <cstdint>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

// #define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../thirdparty/stb/stb_truetype.h"

struct vec2 {
  vec2() = default;
  vec2(int a, int b) : x{(float)a}, y{(float)b} {}
  vec2(float a, float b) : x{a}, y{b} {}
  vec2(double a, double b) : x{(float)a}, y{(float)b} {}

  vec2 operator/(float v) { return {x / v, y / v}; }
  vec2 operator-(float v) { return {x - v, y - v}; }

  vec2 operator-(vec2 v) { return {x - v.x, y - v.y}; }

  float x{};
  float y{};

  float dot(vec2 a) { return x * a.x + y * a.y; }

  friend std::ostream &operator<<(std::ostream &stream, const vec2 &v);
};

std::ostream &operator<<(std::ostream &stream, const vec2 &v) { return stream << "x: " << v.x << " y: " << v.y; }

struct color {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
};

template <typename T> T clamp(T x, T a, T b) { return std::min(std::max(x, a), b); }

int main(int argc, char **argv) {

  int w = 0, h = 0;
  std::ifstream file("font.ppm", std::ios::binary);
  std::string format;
  std::string dim;
  std::string max_value;

  file >> format;
  file >> dim;
  w = std::atoi(dim.c_str());
  file >> dim;
  h = std::atoi(dim.c_str());
  file >> max_value;

  std::vector<uint16_t> sdf_font;
  color pixels[w * h];
  const int num_pixels = w * h;

  size_t num_threads = std::thread::hardware_concurrency();
 
  std::vector<std::pair<int, int>> ranges;
  std::vector<std::thread> threads;
  ranges.resize(num_threads);
  threads.resize(num_threads);

  int work_size = num_pixels / num_threads;

  for (int i = 0; i < num_threads; i++) {
    ranges[i] = {i * work_size, (i + 1) * work_size};
  }

  file.seekg((int)file.tellg() + 1);

  std::cout << file.tellg() << std::endl;
  file.read((char *)&pixels[0], num_pixels * 3);

  std::cout << format << std::endl;
  std::cout << dim << std::endl;
  std::cout << max_value << std::endl;

  sdf_font.resize(num_pixels);

  for (uint16_t &n : sdf_font)
    n = std::numeric_limits<uint16_t>().max();

  int thread_index = 0;
  for (auto &thread : threads) {
    thread = std::thread([thread_index, &ranges, &pixels, &num_pixels, &sdf_font, w, h]() {
      for (int i = ranges[thread_index].first; i < ranges[thread_index].second; i++) {
        if (pixels[i].r >= 1)
          continue;

        int x = i % w;
        int y = i / h;

        vec2 current = vec2(x, y);

        for (int j = 0; j < num_pixels; j++) {
          if (pixels[j].r >= 1) {

            int dx = j % w;
            int dy = j / h;

            vec2 v = vec2(dx, dy);
            vec2 dir = current - v;
            float distance = std::sqrt(dir.dot(dir));

            if (distance < sdf_font[i]) {
              sdf_font[i] = (uint16_t)distance;
            }
          }
        }
      }
    });
    thread_index++;
  }

  for (auto &thread : threads)
    thread.join();

  int max = 0;

  for (auto &n : sdf_font) {
    if (n == std::numeric_limits<uint16_t>().max())
      n = 0;
    if (n > max)
      max = n;
  }

  std::ofstream outImage("ring_sdf.ppm");
  outImage << "P2\n";
  outImage << w << " " << h << "\n";
  outImage << max << "\n";

  for (int i = 0; i < w * h; i++) {
    outImage << (int)max - sdf_font[i] << " ";

    if (i % 255 == 0 && i != 0) {

      outImage << "\n";
    }
  }
}