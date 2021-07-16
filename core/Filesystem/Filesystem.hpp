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
            //std::cout << "trying to scan mounted filesystems" << std::endl;

            std::pair<std::size_t, std::vector<File>> output;
            for (const auto &[id, fs] : mVirtualFilesystems)
            {
                //std::cout << "found mounted fs " << fs->mount_point().Name << std::endl;
                if (fs->status() == VirtualFilesystem::Status::mounted)
                {
                    output.first = fs->Id().get();
                    //std::cout << "fs " << fs->mount_point().Name << " is mounted" << std::endl;
                    //try {
                    std::filesystem::current_path(dir);
                    //} catch(std::exception& e) {
                    //    std::cout << e.what() << " - " << dir << std::endl;
                    //    exit(-1);
                    //}
                    switch (fs->type())
                    {
                    case VirtualFilesystem::Type::directory:
                        //std::cout << std::filesystem::current_path() << std::endl;
                        for (auto &p : std::filesystem::directory_iterator(std::filesystem::current_path()))
                        {
                            File tmpFile(p.path().filename().generic_string(), File::Type::binary);

                            tmpFile.type(File::Type::binary);

                            if (p.is_directory())
                                tmpFile.type(File::Type::directory);
                            else
                                tmpFile.Size(p.file_size());

                            tmpFile.AbsolutePath(p.path().string());
                            output.second.push_back(tmpFile);

                            //std::cout << p.path().filename() << '\n';
                        }
                        break;
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
            
            MountingPoint mountingPoint = {name, path};
            mVirtualFilesystems[name] = std::make_shared<DirectoryFilesystem>(vfs_id, type, mountingPoint);
            mVirtualFilesystems[name]->status(VirtualFilesystem::Status::mounted);
        }

        virtual void unmount()
        {
        }

        VirtualFilesystem_ptr getFilesystem(const std::string &name) {
            return mVirtualFilesystems[name];
        }

    private:
        Filesystem() = default;
        static Filesystem mInstance;
        std::unordered_map<std::string, VirtualFilesystem_ptr> mVirtualFilesystems;
    };
}