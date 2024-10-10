#pragma once

#include "Types.hpp"
#include "core/framework/clock.hpp"
#include "core/framework/easing_functions.hpp"
#include <core/framework/log.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <initializer_list>
#include <span>
#include <vector>

namespace UI {
enum class Action : uint8_t { 
  Scale     = 1 << 0, 
  Translate = 1 << 1, 
  Rotate    = 1 << 2, 
  Color     = 1 << 3, 
  Alpha     = 1 << 4, 
  Type      = 1 << 5, 
  Key       = 1 << 6, 
  None      = 7
};

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

  Key(double duration_, std::initializer_list<Key> k) : duration{duration_}, keys{k} {
    if (k.size() == 1) {
      action = keys[0].action;
      interpolation = keys[0].interpolation;
      value = keys[0].value;
      duration = keys[0].duration;
    } else {
      action = Action::Key;
      interpolation = Interpolation::EaseInSine;
    }

    std::sort(keys.begin(), keys.end(), [](auto &a, Key &b){return a.action > b.action;});

    for (auto &key : keys)
      flags |= (uint8_t) key.action;

  }

  Action action = Action::None;
  Interpolation interpolation = Interpolation::Linear;
  glm::vec4 value{};
  double duration = 1.0;

  void Reset() {
    t = 0.0;
    elapsed = 0.0;
    finished = false;

    for (auto &key : keys)
      key.Reset();
  }

protected:
  double t = 0.0;
  double elapsed = 0.0;
  bool finished = false;
  uint8_t flags = 0;

  std::vector<Key> keys;
  friend struct AnimationManager;
};

struct AnimationManager {

  glm::vec2 accum_transform{};
  double accum_rot = 0.0;
  glm::vec4 current_color{};
  bool color_set = false;

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

  glm::mat2 rot(double t) { return glm::mat2{glm::cos(t), -glm::sin(t), glm::sin(t), glm::cos(t)}; }

  void pre_rotate(UIVertex &vtx, glm::vec2 position, glm::vec2 size) {

    vtx.pos_uv.x -= (position.x);
    vtx.pos_uv.y -= (position.y);

    vtx.pos_uv.x *= (1280.0);
    vtx.pos_uv.y *= (720.0);
  }

  void pre_color(UIVertex &vtx, glm::vec2 position, glm::vec2 size) {
    pre_translate(vtx, position, size);

    if((current_flags & (uint8_t)Action::Translate) == 0) {
      vtx.pos_uv.x += accum_transform.x;
      vtx.pos_uv.y += accum_transform.y;
    }
    if (color_set && (current_flags & (uint8_t)Action::Color) == 0)
      vtx.color = current_color;
  }

  void pre_translate(UIVertex &vtx, glm::vec2 position, glm::vec2 size) {

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

  void post_translate(UIVertex &vtx) {
    if (color_set && (current_flags & (uint8_t)Action::Color) == 0)
      vtx.color = current_color;
  }

  void post_rotate(UIVertex &vtx, glm::vec2 position, glm::vec2 size) {

    vtx.pos_uv.x += position.x;
    vtx.pos_uv.y += position.y;

    if((current_flags & (uint8_t)Action::Translate) == 0) {
      vtx.pos_uv.x += accum_transform.x;
      vtx.pos_uv.y += accum_transform.y;
    }
    if (color_set && (current_flags & (uint8_t)Action::Color) == 0)
      vtx.color = current_color;
  }

  void Add(Key key) { keys.push_back(key); }

  void ProcessKey(Key &key, std::span<UIVertex> &span, bool child_key) {

    if (key.finished || key.elapsed >= key.duration)
      return;

    if (key.action == Action::Key) {
      for (auto &k : key.keys)
        ProcessKey(k, span, true);
    }
    else {
      for (auto &vtx : span) {
        switch (key.action) {
        case Action::Translate: {

          if((current_flags & (uint8_t)Action::Rotate) == 0) 
            pre_translate(vtx, position, size);

          vtx.pos_uv.x = accum_transform.x + (vtx.pos_uv.x + (key.value.x) * key.t);
          vtx.pos_uv.y = accum_transform.y + (vtx.pos_uv.y + (key.value.y) * key.t);

          if((current_flags & (uint8_t)Action::Rotate) == 0) 
           post_translate(vtx);
          break;
        }
        case Action::Rotate: {
          pre_rotate(vtx, position, size);

          glm::vec2 pos = glm::vec2(vtx.pos_uv.x, vtx.pos_uv.y);

          pos = rot((accum_rot / 180.0) * glm::pi<double>()) * rot((key.value.x / 180.0) * key.t * glm::pi<double>()) * pos;

          pos /= glm::vec2(1280, 720);

          vtx.pos_uv.x = pos.x;
          vtx.pos_uv.y = pos.y;

          post_rotate(vtx, position, size);

        } break;
        case Action::Scale: {
        } break;
        case Action::Color:
          if((current_flags & (uint8_t)Action::Rotate) == 0)
            pre_color(vtx, position, size);
          if (!color_set)
            vtx.color = glm::mix(vtx.color, key.value, key.t);
          else
            vtx.color = glm::mix(current_color, key.value, key.t);
          break;
        case Action::Alpha:
          vtx.color.a = glm::mix(vtx.color.a, key.value.a, key.t);
          break;
        case Action::None:
          break;
        case Action::Type:
          break;
        default: break;
        }
      }
    }

    key.elapsed += d_t;

    if (key.elapsed >= key.duration && !key.finished) {
      if (child_key == false) {
        current_key++;
      }
      key.finished = true;
      key.elapsed = 0.0;
      key.t = 0.0;
      if (key.action != Action::Key) {
        if (key.action == Action::Translate) {
          accum_transform.x += key.value.x;
          accum_transform.y += key.value.y;
        } else if (key.action == Action::Rotate) {
          accum_rot += key.value.x;
        } else if (key.action == Action::Color) {
          current_color = key.value;
          color_set = true;
        }
      }
    } else {
      key.t = Interpolate(key.elapsed, key.duration, key.interpolation);
    }
  }

  void Update(std::span<UIVertex> &span, glm::vec2 position, glm::vec2 size) {
    if (keys.size() <= 0)
      return;
    this->position = position;
    this->size = size;

    if (current_key >= keys.size()) {
      current_key = 0;
      accum_transform = {};
      accum_rot = {};
      color_set = false;
      current_color = {};

      for (auto &k : keys)
        k.Reset();
    }

    Key &key = keys[current_key];
    current_flags = key.flags;

    d_t = glm::min(framework::Clock::DeltaSeconds(), 0.016l);

    ProcessKey(key, span, false);
  }

private:
  uint8_t current_flags;
  glm::vec2 position{};
  glm::vec2 size{};
  double d_t = 0.0;
  std::vector<Key> keys;
  std::size_t current_key = 0;
};
} // namespace UI