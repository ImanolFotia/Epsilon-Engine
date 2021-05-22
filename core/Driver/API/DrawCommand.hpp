#pragma once

#include <pch.hpp>

namespace Epsilon::API
{
    class DrawCommand
    {
    public:
        DrawCommand() = delete;
        DrawCommand(const DrawCommand &&) = delete;
        DrawCommand(DrawCommand&&) = delete;

        virtual const std::any & data() = 0;
    };
}