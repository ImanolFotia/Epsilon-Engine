#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace UI {
enum class Action : uint8_t { Translate = 0, Rotate, Scale, Color, Alpha, None};

enum class Interpolation {
  Linear = 0,
  EaseInSine,
  EaseOutSine,
  EaseInOutSine,
  EaseInQuad,
  EaseOutQuad,
  EaseInOutQuad,
  EaseInCubic,
  EaseOutCubic,
  EaseInOutCubic,
  EaseInQuart,
  EaseOutQuart,
  EaseInOutQuart,
  EaseInQuint,
  EaseOutQuint,
  EaseInOutQuint,
  EaseInExpo,
  EaseOutExpo,
  EaseInOutExpo,
  EaseInCirc,
  EaseOutCirc,
  EaseInOutCirc,
  EaseInBack,
  EaseOutBack,
  EaseInOutBack,
  EaseInElastic,
  EaseOutElastic,
  EaseInOutElastic,
  EaseOutBounce,
  EaseInBounce,
  EaseInOutBounce
};

struct Key {
  Action action = Action::None;
  Interpolation interpolation = Interpolation::Linear;
  glm::vec4 value{};
  float duration = 0.0f;
};

struct AnimationManager {
  std::vector<Key> keys;
  void Update() {
    for (auto &key : keys) {
    }
  }
};
} // namespace UI