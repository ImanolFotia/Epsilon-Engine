#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <cstring>
#include "core/framework/def.hpp"

namespace framework {
    class WAVfile {
    public:
        WAVfile() = default;

        ~WAVfile() {}

        std::shared_ptr<unsigned char> Load(const char *fn) {
            unsigned char buffer[4];
            std::ifstream inFILE(fn, std::ios::binary);
            inFILE.read((char *) buffer, 4);

            if (std::strncmp((char *) buffer, "RIFF", 4) != 0) {
                std::cout << " ::AUDIO ERROR:: ---> " << fn << " is not a valid WAVE file." << std::endl;
                return NULL;
            }
            inFILE.read((char *) buffer, 4);
            inFILE.read((char *) buffer, 4); //WAVE
            inFILE.read((char *) buffer, 4); //fmt
            inFILE.read((char *) buffer, 4); //16
            inFILE.read((char *) buffer, 2); //1
            inFILE.read((char *) buffer, 2);
            chan = ByteToInt((char *) buffer, 2);

            inFILE.read((char *) buffer, 4);
            samplerate = ByteToInt((char *) buffer, 4);

            inFILE.read((char *) buffer, 4);
            inFILE.read((char *) buffer, 2);
            inFILE.read((char *) buffer, 2);
            bps = ByteToInt((char *) buffer, 2);

            inFILE.read((char *) buffer, 4); //data
            inFILE.read((char *) buffer, 4);
            size = ByteToInt((char *) buffer, 4);

            std::cout << "chan" << chan << std::endl;
            std::cout << "samplerate" << samplerate << std::endl;
            std::cout << "bps" << bps << std::endl;
            std::cout << "size" << size << std::endl;

            mdata = (std::shared_ptr<unsigned char>) new unsigned char[size];

            inFILE.read((char *) mdata.get(), size);

            return mdata;
        }

        int getNumberOfChannels() {
            return chan;
        }

        int getSampleRate() {
            return samplerate;
        }

        int getBPS() {
            return bps;
        }

        int getFileSize() {
            return size;
        }

        std::shared_ptr<unsigned char> data() {
            return mdata;
        }

    private:
        int chan;
        int samplerate;
        int bps;
        int size;
        std::shared_ptr<unsigned char> mdata;
    };
}
