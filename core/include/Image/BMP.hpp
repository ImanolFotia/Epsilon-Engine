#pragma once

#include <Types.h>

#define BI_RGB              0x00
#define BI_RLE8             0x01
#define BI_RLE4             0x02
#define BI_BITFIELDS        0x03
#define BI_ALPHABITFIELDS   0x06
#define BI_CMYK             0x11
#define BI_CMYKRLE8         0x12
#define BI_CMYKRLE4         0x13

struct BMP_HEADER
{
    SHORT _signature; //4D42
    INT _size; //unreliable
    SHORT _reserved;
    SHORT __reserved;
    INT _offset;
};

struct BITMAP_INFORMATION_HEADER
{
    INT BITMAPINFOHEADER_SIZE; //40
    INT WIDTH;
    INT HEIGHT;
    SHORT NUM_PLANES; //1
    SHORT BPP;
    INT COMPRESSION; //NONE, RLE-8, RLE-4
    INT SIZE; //size of image data in bytes (including padding)
    INT HORIZONTAL_RESOLUTION; //horizontal resolution in pixels per meter (unreliable)
    INT VERTICAL_RESOLUTION; //vertical resolution in pixels per meter (unreliable)
    INT NUM_COLORS; // 0-->N
    INT NUM_IMPORTANT_COLORS; // 0-->N
};
