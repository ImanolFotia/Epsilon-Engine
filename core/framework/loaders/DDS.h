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
	DDS_HEADER* header = nullptr;
	unsigned char* m_pData = nullptr;
	size_t m_pSize{};
	unsigned int m_InternalFormat{};
	unsigned int m_pBlockSize{};

};

