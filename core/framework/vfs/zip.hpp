#ifndef ZIP_H_INCLUDED
#define ZIP_H_INCLUDED

#include <zlib.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <bitset>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>
#include <fstream>
#include <type_traits>
#include <cassert>
#include "cifstream.hpp"

namespace engine {
    constexpr uint32_t LOCAL_FILE_HEADER_SIGNATURE = 0x04034b50;
    constexpr uint32_t DATA_DESCRIPTOR_HEADER_SIGNATURE = 0x08074b50;
    constexpr uint32_t CENTRAL_DIRECTORY_HEADER_SIGNATURE = 0x02014b50;
    constexpr uint32_t END_OF_CENTRAL_DIRECTORY_HEADER_SIGNATURE = 0x06054b50;
#pragma pack(push,1) 
    template <typename U16, typename U32>
    struct LocalFileHeader
    {
        /** offset 0*/ U32 signature; /// 0x04034b50

        /** offset 4*/ U16 minversion;
        /** offset 6*/ U16 flag;

        /** offset 8*/ U16 compmethod;
        /** offset 10*/ U32 lastmodtime;

        /** offset 14*/ U32 CRC32;

        /** offset 20*/ U32 compsize;

        /** offset 24*/ U32 uncompsize;

        /** offset 28*/ U16 filenamelength;
        /** offset 30*/ U16 extrafieldlength;
    };
#pragma pack(pop)
#pragma pack(push,1) 
    template <typename U32>
    struct DataDescriptor
    {
        /** offset 0*/ U32 signature; /// 0x08074b50
        /** offset 4*/ U32 CRC32;
        /** offset 8*/ U32 compsize;
        /** offset 12*/ U32 uncompsize;
    };
#pragma pack(pop)
#pragma pack(push,1) 
    template <typename U16, typename U32>
    struct CentralDirectoryHeader
    {
        /** offset 0*/ U32 signature; /// 0x02014b50

        /** offset 4*/ U16 version;
        /** offset 6*/ U16 minversion;

        /** offset 8*/ U16 flag;
        /** offset 10*/ U16 commethod;

        /** offset 12*/ U16 lastmodtimel;
        /** offset 14*/ U16 lastmoddate;

        /** offset 16*/ U32 CRC32;

        /** offset 20*/ U32 compsize;

        /** offset 24*/ U32 uncomsize;

        /** offset 28*/ U16 filenamelength;
        /** offset 30*/ U16 extrafieldlength;

        /** offset 32*/ U16 filecommentlength;
        /** offset 34*/ U16 filestartsdisk;

        /** offset 36*/ U16 intatt;

        /** offset 38*/ U32 extatt;

        /** offset 42*/ U32 offsettolocalheader;
    };
#pragma pack(pop)
#pragma pack(push,1) 
    template <typename U16, typename U32>
    struct EndOfCentralDirectory
    {
        /** offset 0*/ U32 signature; /// 0x06054b50
        /** offset 4*/ U16 disknum;
        /** offset 6*/ U16 diskcendirstarts;
        /** offset 8*/ U16 numcdr;
        /** offset 10*/ U16 totalcdr;
        /** offset 12*/ U32 cdsize;
        /** offset 16*/ U32 offsetcd;
        /** offset 20*/ U16 commentlength;
    };
#pragma pack(pop)
    class File;

    class Zip
    {
    public:
        Zip() = default;
        Zip(std::string path);

        std::ifstream& getFileHandle()
        {
            return m_Infile;
        }

        std::shared_ptr<cifstream> getFileByName(const std::string& FileName);

        size_t InflateFile(unsigned char* inputFileStream, uInt inputStreamSize, unsigned char* outputFileStream, uInt outputstreamSize, size_t windowBits);

    private:
        size_t getLocalHeader(size_t curroffset);

        size_t getCentralDirectoryHeader(size_t curroffset);

        size_t getEndOfCentralDirectory(size_t curroffset);

        size_t getDataDescriptor(size_t curroffset);

    private:
        std::ifstream m_Infile{};
        std::string m_InfilePath{};
        std::vector<std::shared_ptr<LocalFileHeader<uint16_t, uint32_t>>> m_LocalFileHeaders{};
        std::vector<std::shared_ptr<DataDescriptor<uint32_t>>> m_DataDescriptors{};
        std::vector<std::shared_ptr<CentralDirectoryHeader<uint16_t, uint32_t>>> m_CentralDirectoryHeaders{};
        std::shared_ptr<EndOfCentralDirectory<uint16_t, uint32_t>> m_EndCentralDirectory = nullptr;

        std::unordered_map<std::string, std::shared_ptr<cifstream>> m_FileCollection{};

    protected:
        std::string Compression_Method[20] = {
            "Stored",
            "Shrunk",
            "Reduced with compression factor 1",
            "Reduced with compression factor 2",
            "Reduced with compression factor 3",
            "Reduced with compression factor 4",
            "Imploded",
            "Tokenized",
            "Deflated",
            "Deflate64",
            "IBM TERSE (old)",
            "PKWARE",
            "BZIP2",
            "PKWARE",
            "LZMA (EFS)",
            "PKWARE",
            "PKWARE",
            "PKWARE",
            "IBM TERSE (new)",
            "IBM LZ77" };
    };
}
#endif // ZIP_H_INCLUDED
