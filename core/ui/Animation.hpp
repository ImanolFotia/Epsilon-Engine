#pragma once

#include "Types.hpp"
#include "core/framework/clock.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <span>
#include <vector>

namespace UI {
enum class Action : uint8_t { Translate = 0, Rotate, Scale, Color, Alpha, None };

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
  // Interpolation interpolation = Interpolation::Linear;
  std::function<float(float)> interpolation = [](float x) -> float { return x; }; // default linear
  glm::vec4 value{};
  float duration = 1.0f;

protected:
  float t = 0.0f;
  float elapsed = 0.0f;
  friend struct AnimationManager;
};

struct AnimationManager {

  void Add(Key key) { keys.push_back(key); }

  void Update(std::span<UIVertex> span) {
    if (current_key > keys.size())
      return;

    auto getCenter = [](std::span<UIVertex> span) {
      glm::vec2 c;
      for (auto &vtx : span)
        c += glm::vec2(vtx.pos_uv);
      c /= span.size();
      return c;
    };

    auto sub = [](std::span<UIVertex> span, glm::vec2 p) {
      for (auto &vtx : span) {
        vtx.pos_uv.x -= p.x;
        vtx.pos_uv.y -= p.y;
      }
    };

    auto add = [](std::span<UIVertex> span, glm::vec2 p) {
      for (auto &vtx : span) {
        vtx.pos_uv.x += p.x;
        vtx.pos_uv.y += p.y;
      }
    };

    auto rot = [](float t) {return glm::mat2{glm::cos(t), -glm::sin(t), glm::sin(t), glm::cos(t)};};

    Key &key = keys[current_key];

    for (auto &vtx : span) {
      switch (key.action) {
      case Action::Translate: {
        vtx.pos_uv.x = glm::mix(vtx.pos_uv.x, key.value.x, key.t);
        vtx.pos_uv.y = glm::mix(vtx.pos_uv.y, key.value.y, key.t);
      } break;
      case Action::Rotate: {
        glm::vec2 c = getCenter(span);
        sub(span, c);
        // rotate here
        add(span, c);
      } break;
      case Action::Scale: {
        glm::vec2 c = getCenter(span);
        sub(span, c);
        // scale here
        add(span, c);
      } break;
      case Action::Color:
        vtx.color = glm::mix(vtx.color, key.value, key.t);
        break;
      case Action::Alpha:
        vtx.color.a = glm::mix(vtx.color.a, key.value.a, key.t);
        break;
      case Action::None:
        break;
      }
    }

    key.elapsed = (key.elapsed * key.duration) + framework::Clock::DeltaSeconds();

    key.t = key.interpolation(key.elapsed);

    if (key.t >= 1.0f) {
      current_key++;
      key.t = 0.0f;
    }
  }

private:
  std::vector<Key> keys;
  size_t current_key = 0;
};
} // namespace UI