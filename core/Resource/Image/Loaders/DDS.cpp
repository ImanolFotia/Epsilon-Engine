
#include <pch.hpp>
#include "DDS.h"
#include <Core.hpp>

namespace IO {
    namespace Image {
        DDS::DDS(std::string filename) {
            _header = new DDS_HEADER();

            std::ifstream FILE(filename, std::ios::binary);

            FILE.seekg(0, FILE.beg);
            char* filecode = new char[MAGICNUMBEROFFSET];
            FILE.read(filecode, sizeof(char) * MAGICNUMBEROFFSET);

            std::string str(filecode);


            if (str.compare("DDS ")) {
                std::cout << "Not a DDS File." << std::endl;
                FILE.close();
                return;
            }

            FILE.seekg(MAGICNUMBEROFFSET, FILE.beg);
            FILE.read((char*)_header, sizeof(DDS_HEADER));

            unsigned int bufsize;
            bufsize = _header->dwMipMapCount > 1 ? _header->dwPitchOrLinearSize * 2 : _header->dwPitchOrLinearSize;
            data = (std::shared_ptr<unsigned char>) new unsigned char[bufsize * sizeof(unsigned char)];
            FILE.seekg(sizeof(DDS_HEADER) + MAGICNUMBEROFFSET, FILE.beg);
            FILE.read((char*)data.get(), bufsize);

            FILE.close();

            unsigned int components  = (this->_header->ddspf.dwFourCC == FOURCC_DXT1) ? 3 : 4;

            switch(this->_header->ddspf.dwFourCC) {
            case FOURCC_DXT1:
                this->m_InternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                break;
            case FOURCC_DXT3:
                this->m_InternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                break;
            case FOURCC_DXT5:
                this->m_InternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                break;
            default:
                unsigned char* freeptr = data.get();
                delete[] freeptr;
                freeptr = nullptr;
                return;
            }



        }
    }
}
