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
  Key() {
    interpolation = [](float x) -> float { return x; };
  }

  Key(Action action_, glm::vec4 value_, float duration_) : action(action_), value(value_), duration(duration_) {
    interpolation = [](float x) -> float { return x; };
  }

  Key(Action action_, glm::vec4 value_, std::function<float(float)> interpolation_, float duration_) : action(action_), interpolation(interpolation_), value(value_), duration(duration_) {
    interpolation = [](float x) -> float { return x; };
  }

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

  glm::vec2 accum_transform{};
  float accum_rot = 0.0;

  void Add(Key key) { keys.push_back(key); }

  void Update(std::span<UIVertex>& span, glm::vec2 position, glm::vec2 size) {
    if(keys.size() <= 0) return;

    if (current_key >= keys.size()) {
      current_key = 0;
      accum_transform = {};
    }

    auto rot = [](float t) {return glm::mat2{glm::cos(t), -glm::sin(t), glm::sin(t), glm::cos(t)};};

    Key &key = keys[current_key];

    float aspect_ratio = 720.0 / 1280.0;

    for (auto &vtx : span) {
      switch (key.action) {
      case Action::Translate: {
        vtx.pos_uv.x =  accum_transform.x + (vtx.pos_uv.x + (key.value.x) * key.t);
        vtx.pos_uv.y =  accum_transform.y + (vtx.pos_uv.y + (key.value.y) * key.t);
        break;
      }
      case Action::Rotate: {
        vtx.pos_uv.x -= (position.x);
        vtx.pos_uv.y -= (position.y);

        vtx.pos_uv.x *= ( 1280.0f);
        vtx.pos_uv.y *= ( 720.0f);

        glm::vec2 pos = glm::vec2(vtx.pos_uv.x, vtx.pos_uv.y);
        pos = rot((key.value.x / 180.0f) * key.t* glm::pi<float>()) * pos;

        pos /= glm::vec2(1280, 720);

        vtx.pos_uv.x =  pos.x;
        vtx.pos_uv.y =  pos.y;

        vtx.pos_uv.x += position.x;
        vtx.pos_uv.y += position.y;


        vtx.pos_uv.x +=  accum_transform.x;
        vtx.pos_uv.y +=  accum_transform.y;
      } break;
      case Action::Scale: {
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

    key.elapsed += glm::min(framework::Clock::DeltaSeconds(), 0.016l);

    key.t = key.elapsed / key.duration;//key.interpolation(key.elapsed);

    if (key.elapsed >= key.duration) {
      current_key++;
      key.t = 1.0f;
      key.elapsed = 0.0f;
      if(key.action == Action::Translate) {
      accum_transform.x += key.value.x;
      accum_transform.y += key.value.y;
      } else if(key.action == Action::Rotate) {
        accum_rot += key.value.x;
      }
    }
  }

private:
  std::vector<Key> keys;
  std::size_t current_key = 0;
};
} // namespace UI