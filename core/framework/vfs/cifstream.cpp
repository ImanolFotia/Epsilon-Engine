#include "cifstream.hpp"
#include "filesystem.hpp"
#include <vector>

namespace engine {

	std::unordered_map<std::string, Zip > Filesystem::m_Container;

	int cifstream::read(char* s, unsigned int n)
	{
		try {
			std::ifstream& InFile = Filesystem::m_Container.at(ContainerName).getFileHandle();//std::ifstream(ContainerName, std::ifstream::binary);

			if (!InFile.is_open()) {
				InFile = std::ifstream(ContainerName, std::ifstream::binary);
			}

			//std::ifstream InFile = std::ifstream(ContainerName, std::ifstream::binary);
			std::vector<unsigned char> fileContent;

			InFile.seekg(OffsetInContainer, InFile.beg);

			std::vector<unsigned char> buffer;
			buffer.resize(FileSizeComp);
			InFile.read((char*)(buffer.data()), sizeof(unsigned char) * this->FileSizeComp);

			fileContent.resize(this->FileSizeUncomp);

			switch (compInfo.CompMethod)
			{
			case 0: /**Content is stored*/
				/**Nothing to do here, the content is just stored*/
				fileContent = buffer;
				break;
			case 8: /**Compressed using Deflate*/
				Filesystem::m_Container.at(ContainerName).InflateFile(buffer.data(), this->FileSizeComp, fileContent.data(), this->FileSizeUncomp, -MAX_WBITS);
				break;
			default:
				std::cout << "This compression method is not supported yet." << std::endl;
				return -1;
				break;
			}

			if (buffer.size() == 0)
			{
				return -1;
			}

			std::memcpy(s, &fileContent.data()[m_charposition], n);
			InFile.close();
		}
		catch (std::exception e) {
			throw e;
		}
		//InFile->close();
		return 1;
	}


	std::istream& cifstream::seekg(size_t pos)
	{
		auto& InFile = Filesystem::m_Container.at(ContainerName).getFileHandle();
		m_charposition = pos;
		return InFile.seekg(pos);
	}

	std::istream& cifstream::seekg(size_t off, std::ios_base::seekdir dir)
	{
		auto& InFile = Filesystem::m_Container.at(ContainerName).getFileHandle();
		m_charposition = off;
		return InFile.seekg(off, dir);
	}


	int cifstream::cifstream::is_open()
	{
		auto& InFile = Filesystem::m_Container.at(ContainerName).getFileHandle();
		return InFile.is_open();
	}

	int cifstream::close()
	{
		auto& InFile = Filesystem::m_Container.at(ContainerName).getFileHandle();
		InFile.close();
		m_isopen = false;
		return 0;
	}
}