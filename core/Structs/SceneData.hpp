#pragma once

#include <pch.hpp>

#include <glm/glm.hpp>

struct SceneEntityData
{
    std::vector<uint32_t> entities_ids;
    glm::vec3 position;
};