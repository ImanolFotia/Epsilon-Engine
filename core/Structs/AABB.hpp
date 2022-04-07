#pragma once

#include <glm/glm.hpp>

namespace Epsilon::Structs {
    struct AABB {
        glm::vec3 center;
        glm::vec3 min;
        glm::vec3 max
    }
}