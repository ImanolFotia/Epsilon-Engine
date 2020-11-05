#pragma once

#include <pch.hpp>

namespace Epsilon
{
    enum ErrorType
    {
        NOT_SPECIFIED = 0,
        FILE_NOT_FOUND,
        NO_MEMORY,
        BUFFER_OVERFLOW,
        CORRUPTED_FILE,
        UNKNOWN
    };

    class Error
    {
    public:
        Error() = default;
        Error(const char *what) : mWhat(what){}
        Error(const char *what, enum ErrorType type) : mWhat(what), mType(type){}

        const char *what() { return mWhat; }
        enum ErrorType Type() { return mType; }

    private:
        const char *mWhat;
        enum ErrorType mType;
    };
} // namespace Epsilon