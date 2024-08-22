#pragma once
#include "box.hpp"
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
/** @brief Box structure with some helper functions
 * This box structure is the building block of the octree container
 * The box consists of a size and a position, the later representinf the bottom left corner
 * */
namespace engine {
struct BoundingSphere {
  float radius = 0.0f;
  glm::vec3 position{};
  BoundingSphere(float rad, glm::vec3 pos) : radius{rad}, position{pos} {}

  bool contains(const Box &box) const {
    // if (box.halfSize().x > radius || box.halfSize().y > radius || box.halfSize().z > radius)
    //	return false;

    if (!containsPoint(box.position, position))
      return false;
    if (!containsPoint(box.position + box.size, position))
      return false;
    if (!containsPoint(box.position + glm::vec3(box.size.x, 0.0f, 0.0f), position))
      return false;
    if (!containsPoint(box.position + glm::vec3(0.0f, box.size.y, 0.0f), position))
      return false;
    if (!containsPoint(box.position + glm::vec3(0.0f, 0.0f, box.size.z), position))
      return false;
    if (!containsPoint(box.position + glm::vec3(box.size.x, box.size.y, 0.0f), position))
      return false;
    if (!containsPoint(box.position + glm::vec3(box.size.x, 0.0f, box.size.z), position))
      return false;
    if (!containsPoint(box.position + glm::vec3(0.0f, box.size.y, box.size.z), position))
      return false;
/*
    std::cout << "-------- contains --------" << std::endl;
    std::cout << "box position: " << glm::to_string(box.position) << std::endl;
    std::cout << "sphere position: " << glm::to_string(position) << std::endl;*/

    return true;
  }

  bool containsPoint(glm::vec3 a, glm::vec3 b) const { return glm::distance(a, b) <= radius; }

  bool overlaps(const Box &box) const {
    glm::vec3 center = box.position + box.size * 0.5f;
    glm::vec3 vDelta = glm::max(glm::vec3(0.0), abs(center - position) - box.size * 0.5f);

    float fDistSq = glm::dot(vDelta, vDelta);

    return fDistSq <= radius * radius;
  }
};
} // namespace engine