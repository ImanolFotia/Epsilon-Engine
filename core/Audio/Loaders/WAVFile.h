#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <memory>
#include <Helpers.hpp>

namespace Epsilon
{
    namespace IO
    {
        namespace Audio
        {
            namespace File
            {
                class WAVfile
                {
                public:
                    WAVfile() = default;
                    ~WAVfile() {}

                    std::shared_ptr<char> Load(const char *fn)
                    {
                        char buffer[4];
                        std::ifstream inFILE(fn, std::ios::binary);
                        inFILE.read(buffer, 4);

                        if (strncmp(buffer, "RIFF", 4) != 0)
                        {
                            std::cout << " ::AUDIO ERROR:: ---> " << fn << " is not a valid WAVE file." << std::endl;
                            return NULL;
                        }
                        inFILE.read(buffer, 4);
                        inFILE.read(buffer, 4); //WAVE
                        inFILE.read(buffer, 4); //fmt
                        inFILE.read(buffer, 4); //16
                        inFILE.read(buffer, 2); //1
                        inFILE.read(buffer, 2);
                        chan = Helpers::ByteToInt(buffer, 2);

                        inFILE.read(buffer, 4);
                        samplerate = Helpers::ByteToInt(buffer, 4);

                        inFILE.read(buffer, 4);
                        inFILE.read(buffer, 2);
                        inFILE.read(buffer, 2);
                        bps = Helpers::ByteToInt(buffer, 2);

                        inFILE.read(buffer, 4); //data
                        inFILE.read(buffer, 4);
                        size = Helpers::ByteToInt(buffer, 4);

                        std::shared_ptr<char> data = (std::shared_ptr<char>)new char[size];
                        inFILE.read(data.get(), size);

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
            } // namespace File
        }     // namespace Audio
    }         // namespace IO
} // namespace Epsilon
