#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <cstring>
#include "core/framework/def.hpp"
#include "core/framework/common.hpp"

namespace framework
{
    class WAVfile
    {
    public:
        WAVfile() = default;

        unsigned char *Load(const char *fn)
        {
            unsigned char buffer[4];
            std::ifstream inFILE(fn, std::ios::binary);
            inFILE.read((char *)buffer, 4);

            if (std::strncmp((char *)buffer, "RIFF", 4) != 0)
            {
                Log::Error(" ::AUDIO ERROR:: ---> ", fn, " is not a valid WAVE file.");
                return NULL;
            }
            inFILE.read((char *)buffer, 4);
            inFILE.read((char *)buffer, 4); // WAVE
            inFILE.read((char *)buffer, 4); // fmt
            inFILE.read((char *)buffer, 4); // 16
            inFILE.read((char *)buffer, 2); // 1
            inFILE.read((char *)buffer, 2);
            chan = ByteToInt((char *)buffer, 2);

            inFILE.read((char *)buffer, 4);
            samplerate = ByteToInt((char *)buffer, 4);

            inFILE.read((char *)buffer, 4);
            inFILE.read((char *)buffer, 2);
            inFILE.read((char *)buffer, 2);
            bps = ByteToInt((char *)buffer, 2);

            inFILE.read((char *)buffer, 4); // data
            inFILE.read((char *)buffer, 4);
            size = ByteToInt((char *)buffer, 4);

            mdata = new unsigned char[size];

            inFILE.read((char *)mdata, size);

            return mdata;
        }

        size_t getNumberOfChannels()
        {
            return chan;
        }

        size_t getSampleRate()
        {
            return samplerate;
        }

        size_t getBPS()
        {
            return bps;
        }

        size_t getFileSize()
        {
            return size;
        }

        unsigned char *data()
        {
            return mdata;
        }

    private:
        size_t chan;
        size_t samplerate;
        size_t bps;
        size_t size;
        unsigned char *mdata;
    };
}
