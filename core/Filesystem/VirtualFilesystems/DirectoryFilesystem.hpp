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

            return mEmptyFile;
        }

        virtual bool exists(const std::string &dir) override
        {
            return std::filesystem::exists(mMountPoint.AbsolutePath + "/" + dir);
        }

        virtual bool CreateDirectory(const std::string& name, const std::string path = "") override {
            auto bkp_path = std::filesystem::current_path();
            std::filesystem::current_path(mMountPoint.AbsolutePath);
            std::filesystem::create_directories(path + "/" + name);
            std::filesystem::current_path(bkp_path);

            std::cout << "Created directory: " << mMountPoint.AbsolutePath << path << "/" << name << std::endl;

            return true;
        }

        File mEmptyFile;
    };
}