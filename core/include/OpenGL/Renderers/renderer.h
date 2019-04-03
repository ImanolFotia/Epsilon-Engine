#pragma once

#include <GL/glew.h>

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() {}

    void Start() = 0;
    void End() = 0;

private:

};
