#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstring>


namespace engine {
#ifdef __cplusplus
    extern "C"
    {
#endif

#ifdef BUILDING_VFS_DLL
#define VFS_DLL __declspec(dllexport)
#else
#define VFS_DLL __declspec(dllimport)
#endif
        struct CompressionInfo
        {
            uint16_t CompMethod;
            uint32_t CompressedSize;
            uint32_t UncompressedSize;
        };
#ifdef __cplusplus
    }
#endif

    class cifstream : public std::ifstream
    {
    public:
        cifstream()
        {
            m_isopen = true;

            FileSizeUncomp = 0;
            FileSizeComp = 0;
            FileHeaderIndex = 0;
            OffsetInContainer = 0;
            m_charposition = 0;

            m_isopen = false;
            m_isvirtualfile = false;
            m_isValid = true;
        }


        void PrintFileInfo() {}

    public:
        uint32_t FileSizeUncomp;
        uint32_t FileSizeComp;
        uint32_t FileHeaderIndex;
        uint32_t OffsetInContainer;
        std::string FileName;
        std::string ContainerName{};
        //std::shared_ptr<std::ifstream> realfile;
        uint32_t m_charposition{};
        bool m_isopen;
        bool m_isvirtualfile;
        bool m_isValid;
        bool m_Exists;
        CompressionInfo compInfo;

        std::istream& seekg(size_t pos);

        std::istream& seekg(size_t off, std::ios_base::seekdir dir);

        int read(char* s, unsigned int n);

        int is_open();

        int close();

        bool eof();
    };
}