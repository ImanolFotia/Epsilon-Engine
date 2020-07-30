#pragma once
#include <pch.hpp>

namespace Memory
{
    template <class T>
    inline static T copy_pointer(T pointer)
    {
        return std::move(pointer);
    }

}

