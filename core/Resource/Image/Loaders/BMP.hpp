#pragma once

#include <Types.h>

namespace IO
{
namespace Image
{
#define BI_RGB 0x00
#define BI_RLE8 0x01
#define BI_RLE4 0x02
#define BI_BITFIELDS 0x03
#define BI_ALPHABITFIELDS 0x06
#define BI_CMYK 0x11
#define BI_CMYKRLE8 0x12
#define BI_CMYKRLE4 0x13

struct BMP_HEADER
{
    uint16_t _signature; //4D42
    uint32_t _size;        //unreliable
    uint16_t _reserved;
    uint16_t __reserved;
    uint32_t _offset;
};

struct BITMAP_INFORMATION_HEADER
{
    uint32_t BITMAPINFOHEADER_SIZE; //40
    uint32_t WIDTH;
    uint32_t HEIGHT;
    uint16_t NUM_PLANES; //1
    uint16_t BPP;
    uint32_t COMPRESSION;           //NONE, RLE-8, RLE-4
    uint32_t SIZE;                  //size of image data in bytes (including padding)
    uint32_t HORIZONTAL_RESOLUTION; //horizontal resolution in pixels per meter (unreliable)
    uint32_t VERTICAL_RESOLUTION;   //vertical resolution in pixels per meter (unreliable)
    uint32_t NUM_COLORS;            // 0-->N
    uint32_t NUM_IMPORTANT_COLORS;  // 0-->N
};

} // namespace Image
} // namespace IO