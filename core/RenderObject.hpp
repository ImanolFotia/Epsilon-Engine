#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Epsilon
{
    class RenderObject
    {

    public:
        virtual void Update() = 0;

        virtual void Render() = 0;

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

    protected:
        glm::vec3 mPosition;
        glm::vec3 mScale;
        glm::quat mRotation;
    };
} // namespace Epsilon