#pragma once

namespace Epsilon::Filesystem
{
    class FilesystemId
    {
        std::size_t mId;

    public:
        FilesystemId() = delete;
        FilesystemId(std::size_t id) : mId{id} {};

        std::size_t get() { return mId; }
    };
}