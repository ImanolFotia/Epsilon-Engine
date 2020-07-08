#pragma once

<<<<<<< HEAD
#include <Core.hpp>
=======
#include <GL/glew.h>
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() {}

    virtual void Start() = 0;
    virtual void End() = 0;

private:

};
