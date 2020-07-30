#pragma once

#include <pch.hpp>
#ifdef __linux__
#include <sys/types.h>
#include <dirent.h>
#endif // __linux__


namespace Epsilon
{
    namespace IO
    {
        namespace Filesystem
        {
            class File
            {
            public:
                std::string name;
                uint32_t hash;
                uint32_t type;

                uint32_t generateHash(std::string filename);
            };

            class VirtualFileSystem
            {
            public:
                VirtualFileSystem() = default;
                virtual ~VirtualFileSystem() {}

                virtual File FindFile(const std::string) = 0;

            private:
                bool m_IsZip;
                std::string m_Path;
            };

            class ZipFileSystem : public VirtualFileSystem
            {
            public:
                ZipFileSystem(std::string);
                ~ZipFileSystem();

                File FindFile(const std::string);

            private:
            };

            class PhysicalFileSystem : public VirtualFileSystem
            {
                PhysicalFileSystem(std::string);
                ~PhysicalFileSystem();

                File FindFile(const std::string);

            private:
            };

            class Filesystem
            {
            public:
                const std::vector<std::string> getFileNamesInDirectory(std::string, std::string);

                bool FileExists(std::string);

                bool Mount(std::string, std::string);

                bool FileSystemIsZip(File file);

            private:
                std::unordered_map<std::string, std::string> m_MountedSystems;
                std::vector<File> m_Files;
            };
        } // namespace Filesystem
    }     // namespace IO
} // namespace Epsilon