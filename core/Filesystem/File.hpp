#pragma once

#include <string>
#include <memory>

#include "MountPoint.hpp"
#include "FileData.hpp"

namespace Epsilon::Filesystem {
    class File {

        enum Type {
            plain_text,
            binary
        };

        enum State {
            open = 0,
            closed
        };
        
        std::string mName;
        std::string mAbsolutePath;

        State mState;
        Type mType;

        size_t mSize;

        MountPoint_ptr mMountPoint;

        FileData mFileData;

        public:
            File(const std::string & path, Type type) : mName{path}, mType{type} {}

            void Open(const std::string & path, Type type){}
            void Close() {}

            void Read() {}

        
    };
}