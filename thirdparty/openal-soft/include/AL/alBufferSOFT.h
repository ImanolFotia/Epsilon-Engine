#pragma once
#include <AL/al.h>
#include <AL/alc.h>

#define AL_MONO8_SOFT 0x1100
#define AL_MONO16_SOFT 0x1101
#define AL_MONO32F_SOFT 0x10010
#define AL_STEREO8_SOFT 0x1102
#define AL_STEREO16_SOFT 0x1103
#define AL_STEREO32F_SOFT 0x10011
#define AL_QUAD8_SOFT 0x1204
#define AL_QUAD16_SOFT 0x1205
#define AL_QUAD32F_SOFT 0x1206
#define AL_REAR8_SOFT 0x1207
#define AL_REAR16_SOFT 0x1208
#define AL_REAR32F_SOFT 0x1209
#define AL_5POINT1_8_SOFT 0x120A
#define AL_5POINT1_16_SOFT 0x120B
#define AL_5POINT1_32F_SOFT 0x120C
#define AL_6POINT1_8_SOFT 0x120D
#define AL_6POINT1_16_SOFT 0x120E
#define AL_6POINT1_32F_SOFT 0x120F
#define AL_7POINT1_8_SOFT 0x1210
#define AL_7POINT1_16_SOFT 0x1211
#define AL_7POINT1_32F_SOFT 0x1212

#define AL_MONO_SOFT 0x1500
#define AL_STEREO_SOFT 0x1501
#define AL_QUAD_SOFT 0x1502
#define AL_REAR_SOFT 0x1503
#define AL_5POINT1_SOFT 0x1504
#define AL_6POINT1_SOFT 0x1505
#define AL_7POINT1_SOFT 0x1506

#define AL_BYTE_SOFT 0x1400
#define AL_UNSIGNED_BYTE_SOFT 0x1401
#define AL_SHORT_SOFT 0x1402
#define AL_UNSIGNED_SHORT_SOFT 0x1403
#define AL_INT_SOFT 0x1404
#define AL_UNSIGNED_INT_SOFT 0x1405
#define AL_FLOAT_SOFT 0x1406
#define AL_DOUBLE_SOFT 0x1407
#define AL_BYTE3_SOFT 0x1408
#define AL_UNSIGNED_BYTE3_SOFT 0x1409

#define AL_INTERNAL_FORMAT_SOFT 0x2008
#define AL_BYTE_LENGTH_SOFT 0x2009
#define AL_SAMPLE_LENGTH_SOFT 0x200A

#define AL_SEC_LENGTH_SOFT 0x200B
#define AL_BYTE_RW_OFFSETS_SOFT 0x1031
#define AL_SAMPLE_RW_OFFSETS_SOFT 0x1032

static ALsizei FramesToBytes(ALsizei size, ALenum channels, ALenum type)
{
    switch (channels)
    {
    case AL_MONO_SOFT:
        size *= 1;
        break;
    case AL_STEREO_SOFT:
        size *= 2;
        break;
    case AL_REAR_SOFT:
        size *= 2;
        break;
    case AL_QUAD_SOFT:
        size *= 4;
        break;
    case AL_5POINT1_SOFT:
        size *= 6;
        break;
    case AL_6POINT1_SOFT:
        size *= 7;
        break;
    case AL_7POINT1_SOFT:
        size *= 8;
        break;
    }

    switch (type)
    {
    case AL_BYTE_SOFT:
        size *= sizeof(ALbyte);
        break;
    case AL_UNSIGNED_BYTE_SOFT:
        size *= sizeof(ALubyte);
        break;
    case AL_SHORT_SOFT:
        size *= sizeof(ALshort);
        break;
    case AL_UNSIGNED_SHORT_SOFT:
        size *= sizeof(ALushort);
        break;
    case AL_INT_SOFT:
        size *= sizeof(ALint);
        break;
    case AL_UNSIGNED_INT_SOFT:
        size *= sizeof(ALuint);
        break;
    case AL_FLOAT_SOFT:
        size *= sizeof(ALfloat);
        break;
    case AL_DOUBLE_SOFT:
        size *= sizeof(ALdouble);
        break;
    }

    return size;
}

static void AL_APIENTRY wrap_BufferSamples(ALuint buffer, ALuint samplerate,
                                    ALenum internalformat, ALsizei samples,
                                    ALenum channels, ALenum type,
                                    const ALvoid *data)
{
    alBufferData(buffer, internalformat, data,
                 FramesToBytes(samples, channels, type),
                 samplerate);
}
