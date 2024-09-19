#pragma once

#include "Types.hpp"
#include "core/framework/clock.hpp"
#include "core/framework/easing_functions.hpp"
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
  Key() {}

  Key(Action action_, glm::vec4 value_, double duration_) : action(action_), value(value_), duration(duration_) {}

  Key(Action action_, Interpolation interpolation_, glm::vec4 value_, double duration_)
      : action(action_), interpolation(interpolation_), value(value_), duration(duration_) {}

  Action action = Action::None;
  Interpolation interpolation = Interpolation::Linear;
  glm::vec4 value{};
  double duration = 1.0;

protected:
  double t = 0.0;
  double elapsed = 0.0;
  friend struct AnimationManager;
};

struct AnimationManager {

  glm::vec2 accum_transform{};
  double accum_rot = 0.0;

  inline double Interpolate(double elapsed, double duration, Interpolation interpolation) {
    using namespace framework::easing;
    double t = elapsed / duration;
    switch (interpolation) {
    case Interpolation::Linear:
      return t;
    case Interpolation::EaseInSine:
      return easeInSine(t);
    case Interpolation::EaseOutSine:
      return easeOutSine(t);
    case Interpolation::EaseInOutSine:
      return easeInOutSine(t);
    case Interpolation::EaseInQuad:
      return easeInQuad(t);
    case Interpolation::EaseOutQuad:
      return easeOutQuad(t);
    case Interpolation::EaseInOutQuad:
      return easeInOutQuad(t);
    case Interpolation::EaseInCubic:
      return easeInCubic(t);
    case Interpolation::EaseOutCubic:
      return easeOutCubic(t);
    case Interpolation::EaseInOutCubic:
      return easeInOutCubic(t);
    case Interpolation::EaseInQuart:
      return easeInQuart(t);
    case Interpolation::EaseOutQuart:
      return easeOutQuart(t);
    case Interpolation::EaseInOutQuart:
      return easeInOutQuart(t);
    case Interpolation::EaseInQuint:
      return easeInQuint(t);
    case Interpolation::EaseOutQuint:
      return easeOutQuint(t);
    case Interpolation::EaseInOutQuint:
      return easeInOutQuint(t);
    case Interpolation::EaseInExpo:
      return easeInExpo(t);
    case Interpolation::EaseOutExpo:
      return easeOutExpo(t);
    case Interpolation::EaseInOutExpo:
      return easeInOutExpo(t);
    case Interpolation::EaseInCirc:
      return easeInCirc(t);
    case Interpolation::EaseOutCirc:
      return easeOutCirc(t);
    case Interpolation::EaseInOutCirc:
      return easeInOutCirc(t);
    case Interpolation::EaseInBack:
      return easeInBack(t);
    case Interpolation::EaseOutBack:
      return easeOutBack(t);
    case Interpolation::EaseInOutBack:
      return easeInOutBack(t);
    case Interpolation::EaseInElastic:
      return easeInElastic(t);
    case Interpolation::EaseOutElastic:
      return easeOutElastic(t);
    case Interpolation::EaseInOutElastic:
      return easeInOutElastic(t);
    case Interpolation::EaseOutBounce:
      return easeOutBounce(t);
    case Interpolation::EaseInBounce:
      return easeInBounce(t);
    case Interpolation::EaseInOutBounce:
      return easeInOutBounce(t);
    }

    return t;
  }

  void Add(Key key) { keys.push_back(key); }

  void Update(std::span<UIVertex> &span, glm::vec2 position, glm::vec2 size) {
    if (keys.size() <= 0)
      return;

    if (current_key >= keys.size()) {
      current_key = 0;
      accum_transform = {};
    }

    auto rot = [](double t) { return glm::mat2{glm::cos(t), -glm::sin(t), glm::sin(t), glm::cos(t)}; };

    Key &key = keys[current_key];

    double aspect_ratio = 720.0 / 1280.0;

    for (auto &vtx : span) {
      switch (key.action) {
      case Action::Translate: {
        {
          vtx.pos_uv.x -= (position.x);
          vtx.pos_uv.y -= (position.y);

          vtx.pos_uv.x *= (1280.0);
          vtx.pos_uv.y *= (720.0);

          glm::vec2 pos = glm::vec2(vtx.pos_uv.x, vtx.pos_uv.y);
          
          pos = rot((accum_rot / 180.0) * glm::pi<double>()) * pos;

          pos /= glm::vec2(1280, 720);

          vtx.pos_uv.x = pos.x;
          vtx.pos_uv.y = pos.y;

          vtx.pos_uv.x += position.x;
          vtx.pos_uv.y += position.y;
        }
        vtx.pos_uv.x = accum_transform.x + (vtx.pos_uv.x + (key.value.x) * key.t);
        vtx.pos_uv.y = accum_transform.y + (vtx.pos_uv.y + (key.value.y) * key.t);
        break;
      }
      case Action::Rotate: {
        vtx.pos_uv.x -= (position.x);
        vtx.pos_uv.y -= (position.y);

        vtx.pos_uv.x *= (1280.0);
        vtx.pos_uv.y *= (720.0);

        glm::vec2 pos = glm::vec2(vtx.pos_uv.x, vtx.pos_uv.y);

        pos = rot((accum_rot / 180.0) * glm::pi<double>()) * rot((key.value.x / 180.0) * key.t * glm::pi<double>()) * pos;

        pos /= glm::vec2(1280, 720);

        vtx.pos_uv.x = pos.x;
        vtx.pos_uv.y = pos.y;

        vtx.pos_uv.x += position.x;
        vtx.pos_uv.y += position.y;

        vtx.pos_uv.x += accum_transform.x;
        vtx.pos_uv.y += accum_transform.y;
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


    if (key.elapsed >= key.duration) {
      current_key++;
      key.elapsed = 0.0;
      key.t = 0.0;
      if (key.action == Action::Translate) {
        accum_transform.x += key.value.x;
        accum_transform.y += key.value.y;
      } else if (key.action == Action::Rotate) {
        accum_rot += key.value.x;
      }
    } else {

    key.t = Interpolate(key.elapsed, key.duration, key.interpolation);
    }
  }

private:



  std::vector<Key> keys;
  std::size_t current_key = 0;
};
} // namespace UI