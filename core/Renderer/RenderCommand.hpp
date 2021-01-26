#pragma once

#include "pch.hpp"

namespace Epsilon
{
    class RenderCommand
    {
        enum Type
        {
            OPAQUE = 0,
            TRANSPARENT,
            PARTICLE,
            HAIR
        };

    public:
        RenderCommand() = default;

        RenderCommand(Type t, unsigned int ObjectID)
        {
        }

    private:
        Type mType;
        unsigned int mObjectID;
    };

    using RenderCommand_ptr = std::shared_ptr<RenderCommand>;
} // namespace Epsilon