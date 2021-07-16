#pragma once

#include <string>
#include <memory>

#include "MountPoint.hpp"
#include "FileData.hpp"

namespace Epsilon::Filesystem
{
    class File
    {

    public:
        enum class Type
        {
            plain_text,
            binary,
            directory
        };

        enum class State
        {
            open = 0,
            closed
        };

    public:
        File(const std::string &name, Type type) : mName{name}, mType{type} {}

        void open(const std::string &path, Type type) {}
        void close() {}

        void read() {}

        const std::string & Name() const { return mName; }
        const Type & type() const { return mType; }
        const std::string & AbsolutePath() const { return mAbsolutePath; }
        size_t Size() const { return mSize; }

        void type(Type t) { mType = t; }
        void Size(size_t s) { mSize = s; }

        void AbsolutePath(const std::string& path) { mAbsolutePath = path; }

    private:
        std::string mName;
        std::string mAbsolutePath;

        State mState;
        Type mType;

        size_t mSize;

        MountPoint_ptr mMountPoint;

        FileData mFileData;
    };

}