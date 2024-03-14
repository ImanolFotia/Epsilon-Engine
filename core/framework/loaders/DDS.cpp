#include <iostream>
#include <fstream>
#include <cstring>
#include <core/framework/vfs/filesystem.hpp>
#include "DDS.h"
#include <core/engine/renderer/types.hpp>
#include <glm/glm.hpp>
#include <core/framework/common.hpp>

DDS::DDS(const std::string &filename, int baseLevel = 0) : m_pBaseLevel{baseLevel}
{
	if (!LoadCompressed(filename, baseLevel))
	{
		Load(filename, baseLevel);
	}
}

bool DDS::LoadCompressed(const std::string &filename, int baseLevel = 0)
{

	header = new DDS_HEADER();

	std::string name = filename;

	std::shared_ptr<engine::cifstream> FILE;
	if (filename.starts_with("./"))
	{
		name = filename.substr(2, filename.size());
	}
	if (filename.starts_with("/"))
	{
		name = filename.substr(1, filename.size());
	}
	if (engine::Filesystem::is_present(name.c_str()))
	{
		FILE = engine::Filesystem::open<engine::cifstream>(name);
	}
	else
	{
		IO::Warning("Couldn't find ", name, " in virtual file system\nFalling back to disk...");
		return false;
	}

	FILE->seekg(0, FILE->beg);
	char *filecode = new char[3];
	FILE->read(filecode, sizeof(char) * 3);

	if (strcmp("DDS", filecode) == 0)
	{
		IO::Error(filename, " is not a DDS File.");
		FILE->close();
		return false;
	}

	FILE->seekg(MAGICNUMBEROFFSET, FILE->beg);
	FILE->read((char *)header, sizeof(DDS_HEADER));

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
	case FOURCC_DX10:
	{

		int dataOffset = 148;
		dx10_header = new DX10_HEADER();
		bufsize = 0;
		FILE->seekg(sizeof(DDS_HEADER) + MAGICNUMBEROFFSET, FILE->beg);
		FILE->read((char *)dx10_header, 20);

		if (dx10_header->dxgiFormat == DXGI_FORMAT_BC7_UNORM)
		{
			this->m_InternalFormat = engine::TextureFormat::NON_COLOR_RGBA_BC7;
		}
		else if (dx10_header->dxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB)
		{
			this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC7;
		}
		else if (dx10_header->dxgiFormat == DXGI_FORMAT_BC6H_SF16)
		{
			this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC6HS;
		}
	}
	}

	int levels = std::max(1, header->dwMipMapCount);

	m_pBaseLevel = std::min(m_pBaseLevel, levels - 1);

	if (dx10_header != nullptr)
	{
		for (int j = m_pBaseLevel; j < levels; j++)
		{
			bufsize += calculateSizeBC(m_pBlockSize, header->dwWidth >> j, header->dwHeight >> j);
		}
	}

	for (int i = 0; i < m_pBaseLevel; i++)
		FILE->seekg(calculateSizeBC(m_pBlockSize, header->dwWidth >> i, header->dwHeight >> i), FILE->cur);

	m_pSize = bufsize;

	m_pData = new unsigned char[bufsize * sizeof(unsigned char)];

	if (dx10_header)
		FILE->seekg(148);
	FILE->read((char *)m_pData, bufsize);

	FILE->close();

	return true;
}

void DDS::Load(const std::string &filename, int baseLevel = 0)
{

	header = new DDS_HEADER();

	std::ifstream FILE = std::ifstream(filename, std::ifstream::binary);

	// IO::Error("Loading texture: ", filename);

	FILE.seekg(0, FILE.beg);
	char filecode[4] = {0};
	FILE.read(filecode, sizeof(char) * 4);

	if (strcmp("DDS", filecode) == 0)
	{
		IO::Error(filename, " is not a DDS File.");
		FILE.close();
		return;
	}

	FILE.seekg(MAGICNUMBEROFFSET, FILE.beg);
	FILE.read((char *)header, sizeof(DDS_HEADER));

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
	case FOURCC_DX10:
	{

		int dataOffset = 148;
		dx10_header = new DX10_HEADER();
		bufsize = 0;
		FILE.seekg(sizeof(DDS_HEADER) + MAGICNUMBEROFFSET, FILE.beg);
		FILE.read((char *)dx10_header, 20);

		if (dx10_header->dxgiFormat == DXGI_FORMAT_BC7_UNORM)
		{
			this->m_InternalFormat = engine::TextureFormat::NON_COLOR_RGBA_BC7;
		}
		else if (dx10_header->dxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB)
		{
			this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC7;
		}
		else if (dx10_header->dxgiFormat == DXGI_FORMAT_BC6H_SF16)
		{
			this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC6HS;
		}
		else if (dx10_header->dxgiFormat == DXGI_FORMAT_BC6H_UF16)
		{
			this->m_InternalFormat = engine::TextureFormat::COLOR_RGBA_BC6HU;
		}
	}
	}

	int levels = std::max(1, header->dwMipMapCount);
	m_pBaseLevel = std::min(m_pBaseLevel, levels - 1);

	if (dx10_header != nullptr)
	{
		for (int j = m_pBaseLevel; j < levels; j++)
		{
			bufsize += calculateSizeBC(m_pBlockSize, header->dwWidth >> j, header->dwHeight >> j);
		}
	}

	for (int i = 0; i < m_pBaseLevel; i++)
		FILE.seekg(calculateSizeBC(m_pBlockSize, header->dwWidth >> i, header->dwHeight >> i), FILE.cur);

	m_pSize = bufsize;

	m_pData = new unsigned char[bufsize * sizeof(unsigned char)];

	int i = FILE.tellg();
	// FILE.seekg(dataOffset, FILE.beg);
	FILE.read((char *)m_pData, bufsize);

	FILE.close();
}