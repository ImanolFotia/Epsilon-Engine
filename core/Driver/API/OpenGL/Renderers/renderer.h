#pragma once

#include <Core.hpp>

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() {}

    virtual void Start() = 0;
    virtual void End() = 0;

private:

};
