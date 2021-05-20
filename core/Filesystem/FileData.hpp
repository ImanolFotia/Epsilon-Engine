#pragma once

#include <cstdint>

namespace Epsilon::Filesystem {
    class FileData {
        uint8_t * mData;
        size_t mSize;

        public: 
             uint8_t * Data() {
                 return mData;
             }

             void Data(uint8_t * data) {
                 mData = data;
             }
    };
}