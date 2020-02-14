#pragma once

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <deps/stb_image.h>
#endif

#include <mutex>

static unsigned char* loadImage(const char* p, int * w, int * h, int * c)
{
    std::mutex m;

    unsigned char* pixels;

    m.lock();
    pixels = stbi_load(p, w, h, c, 0);
    m.unlock();

    return pixels;
}
