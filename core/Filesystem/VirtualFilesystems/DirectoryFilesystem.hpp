#pragma once

#include "../VirtualFilesystem.hpp"

#include <filesystem>

namespace Epsilon::Filesystem
{
    class DirectoryFilesystem : public VirtualFilesystem
    {
    public:
    
        DirectoryFilesystem(FilesystemId id, Type t, MountingPoint mp) : VirtualFilesystem(id, t, mp) {
            
        }

        virtual File &find(const std::string &dir) override
        {
            if (exists(dir))
            {
            }
        }

        virtual bool exists(const std::string &dir) override
        {
            return std::filesystem::exists(mMountPoint.AbsolutePath + "/" + dir);
        }
    };
}