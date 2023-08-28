#include <iostream>
#include <fstream>
#include <cstring>
#include "DDS.h"
#include <core/engine/renderer/types.hpp>
#include <glm/glm.hpp>

DDS::DDS(const std::string& filename, unsigned int baseLevel = 0) : m_pBaseLevel{baseLevel}
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

	size_t bufsize = 0;
	size_t blockSize = 0;

	bufsize = header->dwMipMapCount > 1 ? header->dwPitchOrLinearSize * 2 : header->dwPitchOrLinearSize;
	m_pSize = bufsize;
	int dataOffset = sizeof(DDS_HEADER) + MAGICNUMBEROFFSET;

	unsigned int components = (this->header->ddspf.dwFourCC == FOURCC_DXT1) ? 3 : 4;
	m_pBlockSize = 16;
	switch (this->header->ddspf.dwFourCC)
	{
	case FOURCC_DXT1:
		this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC1;
		m_pBlockSize = 8;
		break;
	case FOURCC_DXT3:
		this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC2;
		break;
	case FOURCC_DXT5:
		this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC3;
		break;
	case FOURCC_DX10: {

		int dataOffset = 148;
		dx10_header = new DX10_HEADER();
		bufsize = 0;
		FILE.seekg(sizeof(DDS_HEADER) + MAGICNUMBEROFFSET, FILE.beg);
		FILE.read((char*)dx10_header, 20);

		if (dx10_header->dxgiFormat == DXGI_FORMAT_BC7_UNORM) {
			this->m_InternalFormat = engine::TextureFormat::NON_COLOR_RGBA_BC7;
		}
		else if (dx10_header->dxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB) {
			this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC7;
		}
	}
	}

	int levels = std::max(1, header->dwMipMapCount);

	if (dx10_header != nullptr) {
		for (int j = m_pBaseLevel; j < levels; j++)
		{
			bufsize += calculateSizeBC(m_pBlockSize, header->dwWidth >> j, header->dwHeight >> j);
		}
	}

	for(int i = 0; i < m_pBaseLevel;i++)
		FILE.seekg(calculateSizeBC(m_pBlockSize, header->dwWidth >> i, header->dwHeight >> i), FILE.cur);

	m_pSize = bufsize;

	m_pData = new unsigned char[bufsize * sizeof(unsigned char)];
	//FILE.seekg(dataOffset, FILE.beg);
	FILE.read((char*)m_pData , bufsize);

	FILE.close();



}
