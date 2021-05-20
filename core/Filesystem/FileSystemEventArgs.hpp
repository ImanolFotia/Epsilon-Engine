#pragma once

#include <beacon/beacon.hpp>

namespace Epsilon::Filesystem
{
    enum EventType
    {
        Created = 0,
        Changed,
        Opened,
        Deleted
    };

    class FileSystemEventArgs : public beacon::args
    {
    public:
        FileSystemEventArgs() = default;

        EventType Status() {
            return mEventType;
        }

    private:
        EventType mEventType;
    };
}