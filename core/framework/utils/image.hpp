#pragma once

#include <iostream>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include "../common.hpp"

namespace framework {
static inline unsigned char *load_image_from_file(const char *path, int *width, int *height, int *num_channels) {
  unsigned char *data = stbi_load(path, width, height, num_channels, 4);
  if (data == nullptr) {
    IO::Error("Error loading image from file: ", path);
    return nullptr;
  }
  return data;
}

static inline void free_image_data(unsigned char *data) { stbi_image_free(data); }
} // namespace framework