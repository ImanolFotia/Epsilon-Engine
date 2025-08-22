#pragma once

#include "core/framework/common.hpp"
#include "core/framework/def.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <print>

namespace framework {
class WAVfile {
public:
  WAVfile() = default;
  static bool read_header(WAVfile *wav, std::ifstream &inFILE) {
    unsigned char buffer[4];
    // std::ifstream inFILE(fn, std::ios::binary);
    inFILE.read((char *)buffer, 4);

    if (std::strncmp((char *)buffer, "RIFF", 4) != 0) {
      std::println(" ::AUDIO ERROR:: ---> {} {}", wav->filename, "is not a valid WAVE file.");
      return false;
    }
    inFILE.read((char *)buffer, 4);
    inFILE.read((char *)buffer, 4); // WAVE
    inFILE.read((char *)buffer, 4); // fmt
    inFILE.read((char *)buffer, 4); // 16
    inFILE.read((char *)buffer, 2); // 1
    inFILE.read((char *)buffer, 2);
    wav->chan = ByteToInt((char *)buffer, 2);

    inFILE.read((char *)buffer, 4);
    wav->samplerate = ByteToInt((char *)buffer, 4);

    inFILE.read((char *)buffer, 4);
    inFILE.read((char *)buffer, 2);
    inFILE.read((char *)buffer, 2);
    wav->bps = ByteToInt((char *)buffer, 2);

    inFILE.read((char *)buffer, 4); // data
    inFILE.read((char *)buffer, 4);
    wav->size = ByteToInt((char *)buffer, 4);
    return true;
  }
  unsigned char *Load(const char *fn) {
    filename = fn;
    std::ifstream inFILE(fn, std::ios::binary);
    if (read_header(this, inFILE)) {
      mdata = new unsigned char[size];

      inFILE.read((char *)mdata, size);

      return mdata;
    }
    return nullptr;
  }

  size_t getNumberOfChannels() { return chan; }

  size_t getSampleRate() { return samplerate; }

  size_t getBPS() { return bps; }

  size_t getFileSize() { return size; }

  unsigned char *data() { return mdata; }

private:
  size_t chan;
  size_t samplerate;
  size_t bps;
  size_t size;
  std::string filename;
  unsigned char *mdata;
};
} // namespace framework
