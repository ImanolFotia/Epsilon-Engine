#pragma once

#include <glm/glm.hpp>

namespace Epsilon
{
    class Transform
    {
    private:
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;

    public:
        Transform(glm::vec3 p, glm::vec3 s, glm::quat r) : position(p), scale(s), rotation(r) {}

        auto getPosition() -> glm::vec3
        {
            return position;
        }

        auto getRotation() -> glm::quat
        {
            return rotation;
        }

        auto getScale() -> glm::vec3
        {
            return scale;
        }
        
        auto setPosition(glm::vec3 p) -> void
        {
            position = p;
        }

        auto setRotation(glm::quat r) -> void
        {
            rotation = r;
        }

        auto setScale(glm::vec3 s) -> void
        {
            scale = s;
        }

        auto toMatrix() -> glm::mat4
        {
            glm::mat4 o = glm::mat4(1.0);
            o = glm::translate(o, position);
            o = glm::scale(o, scale);
            o = o * glm::toMat4(glm::normalize(rotation));
            return o;
        }
    };
}