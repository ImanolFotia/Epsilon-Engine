#pragma once

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace framework
{
    static inline unsigned char *load_image_from_file(const char *path, int *width, int *height, int *num_channels)
    {
        unsigned char *data = stbi_load(path, width, height, num_channels, 0);
        if (data == nullptr)
        {
            std::cout << "Error loading texture from file: " << path << std::endl;
            return nullptr;
        }
        return data;
    }

    static inline void free_image_data(unsigned char* data) {
        stbi_image_free(data);
    }
}