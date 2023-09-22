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

    class VFS_DLL cifstream
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
        ~cifstream() {}

        void PrintFileInfo() {}

    public:
        uint32_t FileSizeUncomp;
        uint32_t FileSizeComp;
        uint32_t FileHeaderIndex;
        uint32_t OffsetInContainer;
        std::string FileName;
        std::string ContainerName;
        std::ifstream realfile;
        uint32_t m_charposition;
        bool m_isopen;
        bool m_isvirtualfile;
        bool m_isValid;
        CompressionInfo compInfo;

        template <typename T>
        int seekg(T pos)
        {
            m_charposition = pos;
            return 1;
        }

        template <typename T>
        int seekg(T off, std::ios_base::seekdir dir)
        {
            m_charposition = off;
            return 1;
        }

        int read(char* s, unsigned int n);

        int is_open()
        {
            return m_isopen;
        }

        int close()
        {
            m_isopen = false;
        }

        bool eof()
        {
            if (m_charposition >= FileSizeUncomp)
                return true;
            else
                return false;
        }
    };
}
