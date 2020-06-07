#pragma once
#include "DDSFile.h"
#include <memory>
namespace IO {
    namespace Image {
        class DDS {
        public:

            DDS(std::string);

            ~DDS() {}

            const DDS_HEADER* header() const {
                return _header;
            }

            const std::shared_ptr<unsigned char> Data() const {
                return data;
            }

            const unsigned int InternalFormat() const {
                return m_InternalFormat;
            }

        private:
            DDS_HEADER* _header;
            std::shared_ptr<unsigned char> data;
            unsigned int m_InternalFormat;

        };
    }
}

