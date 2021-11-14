#pragma once

#include "Context.hpp"

#include <unordered_map>

namespace Epsilon::API
{

    class Renderer
    {
        virtual void Draw(PRIMITIVE, unsigned, unsigned) = 0;

        virtual void Enable(STATE) = 0;
        virtual void Disable(STATE) = 0;

        std::unordered_map<STATE, bool> StateCache;
    };

};