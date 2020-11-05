#pragma once
#include <pch.hpp>
#include <Helpers.hpp>

namespace Epsilon
{
    namespace Resources
    {
        class WAVfile
        {
        public:
            WAVfile() = default;
            ~WAVfile() {}

            std::shared_ptr<unsigned char> Load(const char *fn)
            {
                unsigned char buffer[4];
                std::ifstream inFILE(fn, std::ios::binary);
                inFILE.read((char *)buffer, 4);

                if (strncmp((char *)buffer, "RIFF", 4) != 0)
                {
                    std::cout << " ::AUDIO ERROR:: ---> " << fn << " is not a valid WAVE file." << std::endl;
                    return NULL;
                }
                inFILE.read((char *)buffer, 4);
                inFILE.read((char *)buffer, 4); //WAVE
                inFILE.read((char *)buffer, 4); //fmt
                inFILE.read((char *)buffer, 4); //16
                inFILE.read((char *)buffer, 2); //1
                inFILE.read((char *)buffer, 2);
                chan = Helpers::ByteToInt((char *)buffer, 2);

                inFILE.read((char *)buffer, 4);
                samplerate = Helpers::ByteToInt((char *)buffer, 4);

                inFILE.read((char *)buffer, 4);
                inFILE.read((char *)buffer, 2);
                inFILE.read((char *)buffer, 2);
                bps = Helpers::ByteToInt((char *)buffer, 2);

                inFILE.read((char *)buffer, 4); //data
                inFILE.read((char *)buffer, 4);
                size = Helpers::ByteToInt((char *)buffer, 4);

                std::shared_ptr<unsigned char> data = (std::shared_ptr<unsigned char>)new unsigned char[size];

                inFILE.read((char *)data.get(), size);

                return data;
            }

            int getNumberOfChannels()
            {
                return chan;
            }

            int getSampleRate()
            {
                return samplerate;
            }

            int getBPS()
            {
                return bps;
            }

            int getFileSize()
            {
                return size;
            }

        private:
            int chan;
            int samplerate;
            int bps;
            int size;
        };
    } // namespace Resources
} // namespace Epsilon
