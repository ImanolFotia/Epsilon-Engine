#pragma once
#include "DDSFile.h"
#include <memory>
class DDS
{
public:

	DDS(const std::string&);

	~DDS() {
		delete header;
	}


	uint32_t mipLevels() {
		return header->dwMipMapCount;
	}

	uint32_t width() {
		return header->dwWidth;
	}

	uint32_t height() {
		return header->dwHeight;
	}

	size_t size() {
		return m_pSize;
	}

	unsigned char* data() const { 
		return m_pData;
	}

	const unsigned int format() const { 
		return m_InternalFormat; 
	}

	unsigned int blockSize() {
		return m_pBlockSize;
	}

	static void free_data(unsigned char* iData) {
		if(iData != nullptr)
			delete iData;
	}


private:
	int calculateSizeBC(int blockSize, int width, int height)
	{
		return std::max(1, ((height + 3) / 4)) * calculatePitchBC(blockSize, width);
	}

	int calculatePitchBC(int blockSize, int width)
	{
		return std::max(1, ((width + 3) / 4)) * blockSize;
	}

	DDS_HEADER* header = nullptr;
	DX10_HEADER* dx10_header = nullptr;
	unsigned char* m_pData = nullptr;
	size_t m_pSize{};
	unsigned int m_InternalFormat{};
	unsigned int m_pBlockSize{};

};
