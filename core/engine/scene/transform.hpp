#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace engine {

struct Transform {
  unsigned int id = 0;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 prev_position = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);
  glm::vec3 prev_scale = glm::vec3(1.0f);
  glm::vec3 target_scale = glm::vec3(1.0f);
  float angle = 0;
  glm::quat orientation{};
  glm::mat4 model_matrix = glm::mat4(1.0f);
  float lifetime = 0;
  bool grows = false;
  int callback_index = 0;
};

} // namespace engine