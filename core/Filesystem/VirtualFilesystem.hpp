#pragma once

#include <pch.hpp>

#include "MountPoint.hpp"
#include "File.hpp"
#include "FilesystemId.hpp"

namespace Epsilon::Filesystem
{
    class VirtualFilesystem
    {
    public:
        enum Type
        {
            directory = 0,
            network,
            zip
        };

        enum class Status
        {
            mounted = 0,
            unmounted
        };

    protected:
        MountingPoint mMountPoint;
        bool mIsCompressed;
        Type mType;
        FilesystemId mId;
        Status mStatus;

    public:
        /*constructors*/
        VirtualFilesystem(FilesystemId id, Type t, MountingPoint mp) : mId{id}, mType{t}, mMountPoint{mp} {

        }

        VirtualFilesystem(const VirtualFilesystem &) = delete;
        VirtualFilesystem(VirtualFilesystem &&) = delete;

        virtual File &find(const std::string& dir) = 0;

        virtual bool exists(const std::string& dir) = 0;

        void status(Status s)
        {
            mStatus = s;
        }

        Status status()
        {
            return mStatus;
        }

        Type type()
        {
            return mType;
        }

        FilesystemId Id() {
            return mId;
        }

        MountingPoint mount_point() {
            return mMountPoint;
        }

    };
}