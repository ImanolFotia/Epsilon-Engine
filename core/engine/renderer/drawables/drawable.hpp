#pragma once

#include "glm/glm.hpp"
#include <core/common/common.hpp>

#include "../types.hpp"

namespace engine
{
    enum DrawableType
    {
        TRIANGLE_MESH = 0,
        SPHERE,
        CUBE,
        PLANE,
        TOROID,
        CYLINDER,
        CURVE,
        CUBIC_BEZIER,
        CUADRATIC_BEZIER,
        BEZIER,
        PATCH,
        QUAD,
        UNDEFINED
    };

    class Drawable
    {
    public:
        virtual common::Mesh &data() { return m_pMesh; }

        DrawableType getType() { return m_pType; }

    protected:
        common::Mesh m_pMesh;
        DrawableType m_pType = UNDEFINED;
    };
} // namespace Renderer