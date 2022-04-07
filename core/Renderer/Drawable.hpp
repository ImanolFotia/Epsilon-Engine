#pragma once

#include <pch.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Types.h>

#include <Shader.h>

namespace Epsilon::Renderer
{
    
    enum DrawableType {
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
        UNDEFINED
    };

    class Drawable
    {

    public:
        virtual void Update() = 0;

        virtual void Render() = 0;

        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) = 0;

        virtual glm::vec3 getPosition()
        {
            return mPosition;
        }

        virtual glm::vec3 getScale()
        {
            return mScale;
        }

        virtual glm::quat getRotation()
        {
            return mRotation;
        }

        auto getType() -> DrawableType {
            return type;
        }

    protected:
        glm::vec3 mPosition;
        glm::vec3 mScale;
        glm::quat mRotation;

        DrawableType type = DrawableType::UNDEFINED;


    };
} // namespace Renderer