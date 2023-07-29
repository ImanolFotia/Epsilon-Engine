#include <iostream>
#include <fstream>
#include <cstring>
#include "DDS.h"
#include <core/engine/renderer/types.hpp>

DDS::DDS(const std::string& filename)
{
    header = new DDS_HEADER();

    std::ifstream FILE(filename.c_str(), std::ios::binary);

    FILE.seekg(0, FILE.beg);
    char* filecode = new char[MAGICNUMBEROFFSET];
	FILE.read(filecode, sizeof(char) * MAGICNUMBEROFFSET);

    std::string str(filecode);
	str.resize(3);


	if (str.compare("DDS")) {
            std::cout << "Not a DDS File." << std::endl;
            FILE.close();
            return;
	}

    FILE.seekg(MAGICNUMBEROFFSET, FILE.beg);
    FILE.read((char*)header, sizeof(DDS_HEADER));

	size_t bufsize;
	bufsize = header->dwMipMapCount > 1 ? header->dwPitchOrLinearSize * 2 : header->dwPitchOrLinearSize;
	m_pSize = bufsize;
	m_pData = new unsigned char[bufsize * sizeof(unsigned char)];
	FILE.seekg(sizeof(DDS_HEADER) + MAGICNUMBEROFFSET , FILE.beg);
    FILE.read((char*)m_pData, bufsize);

    FILE.close();

    unsigned int components  = (this->header->ddspf.dwFourCC == FOURCC_DXT1) ? 3 : 4;

	switch(this->header->ddspf.dwFourCC)
	{
	case FOURCC_DXT1:
		this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC1;
		break;
	case FOURCC_DXT3:
		this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC2;
		break;
	case FOURCC_DXT5:
		this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC3;
		break;
	}


	m_pBlockSize = (m_InternalFormat == engine::TextureFormat::COLOR_RGBA_BC1) ? 8 : 16;

}
