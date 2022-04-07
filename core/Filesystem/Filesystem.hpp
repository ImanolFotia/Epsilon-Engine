#pragma once

#include <vector>
#include <Log.h>

#include "File.hpp"
#include "FileSystemEventArgs.hpp"
#include "VirtualFilesystem.hpp"
#include "VirtualFilesystems/DirectoryFilesystem.hpp"
#include "Watcher.hpp"

namespace Epsilon::Filesystem
{
    class Filesystem final
    {
        using VirtualFilesystem_ptr = std::shared_ptr<VirtualFilesystem>;

    public:
        Filesystem(Filesystem &&) = delete;
        Filesystem(const Filesystem &) = delete;

        static Filesystem &Instance()
        {
            return mInstance;
        }

        std::pair<std::size_t, std::vector<File>> list(const std::string &dir = ".")
        {
            std::pair<std::size_t, std::vector<File>> output;
            for (const auto &[id, fs] : mVirtualFilesystems)
            {
                if (fs->status() == VirtualFilesystem::Status::mounted)
                {
                    output.first = fs->Id().get();
                    std::filesystem::current_path(dir);
                    switch (fs->type())
                    {
                    case VirtualFilesystem::Type::directory:
                        for (auto &p : std::filesystem::directory_iterator(std::filesystem::current_path()))
                        {
                            File tmpFile(p.path().filename().generic_string(), File::Type::binary);

                            tmpFile.type(File::Type::binary);

                            if (p.is_directory())
                            {
                                tmpFile.type(File::Type::directory);
                            }
                            else
                            {
                                tmpFile.Size(p.file_size());
                            }

                            tmpFile.AbsolutePath(p.path().string());
                            output.second.push_back(tmpFile);
                        }
                        break;
                        default: break;
                    }
                }
            }

            return output;
        }

        virtual void getFile()
        {
        }

        virtual void getAllFiles() {}
        

        virtual void find()
        {
        }

        virtual void seek()
        {
        }

        virtual void read()
        {
        }

        virtual void watch()
        {
        }

        virtual void close()
        {
        }

        virtual void mount(const std::string &path, VirtualFilesystem::Type type, const std::string &name)
        {
            std::size_t vfs_id = std::hash<std::string>{}(name);

            if(!std::filesystem::exists(path)) {
                std::filesystem::create_directories(path);
            }

            MountingPoint mountingPoint = {name, path};
            mVirtualFilesystems[name] = std::make_shared<DirectoryFilesystem>(vfs_id, type, mountingPoint);
            mVirtualFilesystems[name]->status(VirtualFilesystem::Status::mounted);
        }

        virtual void unmount()
        {
        }

        bool createDirectory(const std::string& vfs_name, const std::string &path) {
            
            auto dir_bkp = std::filesystem::current_path();
            auto vfs = mVirtualFilesystems[vfs_name];

            if(vfs->status() != VirtualFilesystem::Status::mounted) return false;

            std::filesystem::current_path(vfs->mount_point().AbsolutePath);

            auto path_to_create = path;

            if(!std::filesystem::exists(path)) {
                std::filesystem::create_directories(path);
            } else {
                std::cout << "Directory already exists: " << path << std::endl;
            }

            std::filesystem::current_path(dir_bkp);
        }

        VirtualFilesystem_ptr getFilesystem(const std::string &name)
        {
            if(mVirtualFilesystems.contains(name))
            return mVirtualFilesystems[name];
            return nullptr;
        }

    private:
        Filesystem() = default;
        static Filesystem mInstance;
        std::unordered_map<std::string, VirtualFilesystem_ptr> mVirtualFilesystems;
    };
}