#include "zip.hpp"
#include "cifstream.hpp"

#include <core/framework/common.hpp>

namespace engine
{
	Zip::Zip(std::string path)
	{

		m_InfilePath = path;
		m_Infile = std::ifstream(m_InfilePath, std::ifstream::binary);

		assert(m_Infile.is_open());

		char ZipIdentifier[2];
		m_Infile.seekg(0, m_Infile.beg);
		m_Infile.read((char *)&ZipIdentifier, 2 * sizeof(char));

		if (ZipIdentifier[0] != 'P' && ZipIdentifier[1] != 'K')
		{
			IO::Error("Attempted to mount a not zip file... aborting.");
			return;
		}

		size_t currentOffset = 0;
		m_EndCentralDirectory = nullptr;

		auto checkSignature = [this, &currentOffset](uint32_t signature) -> size_t
		{
			switch (signature)
			{
			case LOCAL_FILE_HEADER_SIGNATURE:
				return this->getLocalHeader(currentOffset);
				break;

			case DATA_DESCRIPTOR_HEADER_SIGNATURE:
				return this->getDataDescriptor(currentOffset);
				break;

			case CENTRAL_DIRECTORY_HEADER_SIGNATURE:
				return this->getCentralDirectoryHeader(currentOffset);
				break;

			case END_OF_CENTRAL_DIRECTORY_HEADER_SIGNATURE:
				return this->getEndOfCentralDirectory(currentOffset);
				break;

			default:
				return currentOffset;
				break;
			}
		};

		auto getSignature = [this](uint32_t offset) -> uint32_t
		{
			if (!m_Infile.is_open())
			{
				return false;
			}

			uint32_t buffer = 0;
			m_Infile.seekg(offset, m_Infile.beg);
			m_Infile.read((char *)&buffer, sizeof(uint32_t));
			// std::cout << std::hex << buffer << std::dec << std::endl;
			// std::cout << "Offset: " << offset << std::endl;

			return buffer;
		};

		do
		{
			uint32_t signature = getSignature(currentOffset);

			currentOffset = checkSignature(signature);

		} while (
			!m_Infile.eof() &&
			m_EndCentralDirectory == nullptr); /**End while loop*/
		m_Infile.seekg(0);
		m_Infile.close();
	}

	std::shared_ptr<cifstream> Zip::getFileByName(const std::string &FileName)
	{

		std::shared_ptr<cifstream> tmpFile;

		if (m_FileCollection.contains(FileName))
		{

			tmpFile = m_FileCollection[FileName];

			CompressionInfo compInfo;

			auto RetrieveCompression = [&]() -> void
			{
				compInfo.CompMethod = m_LocalFileHeaders.at(tmpFile->FileHeaderIndex)->compmethod;
				compInfo.CompressedSize = tmpFile->FileSizeComp;
				compInfo.UncompressedSize = tmpFile->FileSizeUncomp;
			};

			RetrieveCompression();

			tmpFile->compInfo = compInfo;
			tmpFile->FileName = FileName;
			tmpFile->m_Exists = true;

			return tmpFile;
		}

		std::shared_ptr<cifstream> oFile = std::make_shared<cifstream>();
		oFile->m_isValid = false;
		oFile->m_Exists = false;
		return oFile;
	}

	int Zip::InflateFile(unsigned char *inputFileStream, uInt inputStreamSize, unsigned char *outputFileStream, uInt outputstreamSize, size_t windowBits)
	{
		z_stream infstream;
		infstream.zalloc = Z_NULL;
		infstream.zfree = Z_NULL;
		infstream.opaque = Z_NULL;
		// setup "b" as the input and "c" as the compressed output
		infstream.avail_in = (uInt)(inputStreamSize);	// size of input
		infstream.next_in = (Bytef *)inputFileStream;	// input char array
		infstream.avail_out = (uInt)outputstreamSize;	// size of output
		infstream.next_out = (Bytef *)outputFileStream; // output char array

		int error = 0;

		auto checkError = [](int error) -> int
		{
			switch (error)
			{
			case Z_NEED_DICT:
				IO::Error("Need Dictionary");
				return error;
				break;
			case Z_DATA_ERROR:
				IO::Error("Data error");
				return error;
				break;
			case Z_MEM_ERROR:
				IO::Error("Memory error");
				return error;
				break;
			default:
				return error;
				break;
			}
		};
		// the actual DE-compression work.
		inflateInit2(&infstream, windowBits);

		error = checkError(inflate(&infstream, Z_NO_FLUSH));

		inflateEnd(&infstream);

		return error;
	}

	size_t Zip::getLocalHeader(size_t curroffset)
	{

		std::shared_ptr<LocalFileHeader<uint16_t, uint32_t>> localheader = std::make_shared<LocalFileHeader<uint16_t, uint32_t>>();

		std::shared_ptr<cifstream> tmpFile = std::make_shared<cifstream>();

		m_Infile.seekg(curroffset, m_Infile.beg);
		m_Infile.read(reinterpret_cast<char *>(localheader.get()), sizeof(char) * 30);

		std::vector<unsigned char> buffer;
		buffer.resize(localheader->filenamelength);
		m_Infile.read(reinterpret_cast<char *>(buffer.data()), sizeof(unsigned char) * localheader->filenamelength);

		tmpFile->FileName = std::string(&buffer.data()[0], &buffer.data()[localheader->filenamelength]);

		buffer.clear();
		buffer.resize(localheader->uncompsize);

		m_Infile.read(reinterpret_cast<char *>(buffer.data()), sizeof(unsigned char) * localheader->uncompsize);

		tmpFile->OffsetInContainer = 30 + sizeof(unsigned char) * localheader->filenamelength + curroffset;
		tmpFile->FileSizeComp = localheader->compsize;
		tmpFile->FileSizeUncomp = localheader->uncompsize;
		tmpFile->ContainerName = m_InfilePath;
		tmpFile->FileHeaderIndex = m_LocalFileHeaders.size();
		tmpFile->m_isValid = true;
		// std::cout << "Name: " << tmpFile->FileName  << std::endl;
		// std::cout << "Index for this file: " << tmpFile->FileHeaderIndex  << std::endl;

		m_FileCollection[tmpFile->FileName] = tmpFile;

		m_LocalFileHeaders.push_back(localheader);

		curroffset += localheader->filenamelength +
					  localheader->compsize +
					  localheader->extrafieldlength +
					  30;

		return curroffset;
	}

	size_t Zip::getCentralDirectoryHeader(size_t curroffset)
	{
		std::shared_ptr<CentralDirectoryHeader<uint16_t, uint32_t>> cdh = std::make_shared<CentralDirectoryHeader<uint16_t, uint32_t>>();

		m_Infile.seekg(curroffset, m_Infile.beg);
		m_Infile.read(reinterpret_cast<char *>(cdh.get()), sizeof(char) * 46);

		m_CentralDirectoryHeaders.push_back(cdh);

		curroffset += cdh->extrafieldlength + cdh->filecommentlength + cdh->filenamelength + 46;

		return curroffset;
	}

	size_t Zip::getEndOfCentralDirectory(size_t curroffset)
	{
		std::shared_ptr<EndOfCentralDirectory<uint16_t, uint32_t>> eocd = std::make_shared<EndOfCentralDirectory<uint16_t, uint32_t>>();

		m_Infile.seekg(curroffset, m_Infile.beg);
		m_Infile.read(reinterpret_cast<char *>(eocd.get()), sizeof(char) * 20);

		m_EndCentralDirectory = eocd;

		curroffset += eocd->commentlength + 20;

		return curroffset;
	}

	size_t Zip::getDataDescriptor(size_t curroffset)
	{
		std::shared_ptr<DataDescriptor<uint32_t>> DD = std::make_shared<DataDescriptor<uint32_t>>();

		m_Infile.seekg(curroffset, m_Infile.beg);
		m_Infile.read(reinterpret_cast<char *>(DD.get()), sizeof(char) * 20);

		m_DataDescriptors.push_back(DD);

		curroffset += 16;

		return curroffset;
	}
}
