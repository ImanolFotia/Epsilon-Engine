#pragma once

#include <Core.hpp>
#include <glm/glm.hpp>
namespace Epsilon
{
    class GIProbe
    {
    public:
        GIProbe()
        {
        }

        virtual void Begin(int index) = 0;
        virtual void End() = 0;

    protected:
        GLuint mTexture = 0;
        glm::vec3 mPosition;
    };
} // namespace Epsilon