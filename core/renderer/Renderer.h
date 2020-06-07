#pragma once

#include <GL/glew.h>

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    virtual void beginRender() = 0;
    virtual void endRender() = 0;

private:
    short WIDTH, HEIGHT;

protected:

};

