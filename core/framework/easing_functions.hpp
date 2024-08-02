#pragma once

#include <glm/glm.hpp>

namespace framework::easing {
template <typename T = float> static T easeInSine(T x) { return 1.0 - glm::cos((x * glm::pi<T>()) * 0.5); }

template <typename T = float> static T easeOutSine(T x) { return glm::cos((x * glm::pi<T>()) * 0.5); }

template <typename T = float> static T easeInOutSine(T x) { return -(glm::cos(x * glm::pi<T>()) - 1.0) * 0.5; }

template <typename T = float> static T easeInQuad(T x) { return x * x; }

template <typename T = float> static T easeOutQuad(T x) { return 1.0 - (1.0 - x) * (1.0 - x); }

template <typename T = float> static T easeInOutQuad(T x) { return x < 0.5 ? 2.0 * x * x : 1.0 - glm::pow(-2.0 * x + 2.0, 2.0) * 0.5; }

template <typename T = float> static T easeInCubic(T x) { return x * x * x; }

template <typename T = float> static T easeOutCubic(T x) { return 1.0 - glm::pow(1.0 - x, 3.0); }

template <typename T = float> static T easeInOutCubic(T x) { return x < 0.5 ? 4.0 * x * x * x : 1.0 - glm::pow(-2.0 * x + 2.0, 3.0) * 0.5; }

template <typename T = float> static T easeInQuart(T x) { return x * x * x * x; }

template <typename T = float> static T easeOutQuart(T x) { return 1.0 - glm::pow(1.0 - x, 4.0); }

template <typename T = float> static T easeInOutQuart(T x) { return x < 0.5 ? 8 * x * x * x * x : 1.0 - glm::pow(-2.0 * x + 2.0, 4.0) * 0.5; }

template <typename T = float> static T easeInQuint(T x) { return x * x * x * x * x; }

template <typename T = float> static T easeOutQuint(T x) { return 1.0 - glm::pow(1.0 - x, 5.0); }

template <typename T = float> static T easeInOutQuint(T x) { return x < 0.5 ? 16 * x * x * x * x * x : 1.0 - glm::pow(-2.0 * x + 2.0, 5.0) * 0.5; }

template <typename T = float> static T easeInExpo(T x) { return x == 0.0 ? 0.0 : glm::pow(2.0, 10.0 * x - 10.0); }

template <typename T = float> static T easeOutExpo(T x) { return x == 1.0 ? 1.0 : 1.0 - glm::pow(2.0, -10.0 * x); }

template <typename T = float> static T easeInOutExpo(T x) {
  return x == 0.0 ? 0.0 : x == 1.0 ? 1.0 : x < 0.5 ? glm::pow(2.0, 20.0 * x - 10.0) / 2.0 : (2.0 - glm::pow(2.0, -20.0 * x + 10.0)) / 2.0;
}

template <typename T = float> static T easeInCirc(T x) { return 1.0 - glm::sqrt(1.0 - glm::pow(x, 2.0)); }

template <typename T = float> static T easeOutCirc(T x) { return glm::sqrt(1.0 - glm::pow(x - 1.0, 2.0)); }

template <typename T = float> static T easeInOutCirc(T x) {
  return x < 0.5 ? (1.0 - glm::sqrt(1.0 - glm::pow(2.0 * x, 2.0))) / 2.0 : (glm::sqrt(1.0 - glm::pow(-2.0 * x + 2.0, 2.0)) + 1.0) / 2.0;
}

template <typename T = float> static T easeInBack(T x) {
  const double c1 = 1.70158;
  const double c3 = c1 + 1.0;
  return c3 * x * x * x - c1 * x * x;
}

template <typename T = float> static T easeOutBack(T x) {
  const double c1 = 1.70158;
  const double c3 = c1 + 1.0;
  return 1.0 + c3 * glm::pow(x - 1.0, 3.0) + c1 * glm::pow(x - 1.0, 2.0);
}

template <typename T = float> static T easeInOutBack(T x) {
  const double c1 = 1.70158;
  const double c2 = c1 * 1.525;
  return x < 0.5 ? (glm::pow(2.0 * x, 2) * ((c2 + 1.0) * 2.0 * x - c2)) / 2.0 : (glm::pow(2.0 * x - 2.0, 2) * ((c2 + 1.0) * (x * 2.0 - 2.0) + c2)) / 2.0;
}

template <typename T = float> static T easeInElastic(T x) {
  const double c4 = (2.0 * glm::pi<T>()) / 3.0;
  return x == 0.0 ? 0.0 : x == 1 ? 1.0 : -glm::pow(2.0, 10.0 * x - 10.0) * glm::sin((x * 10.0 - 10.75) * c4);
}

template <typename T = float> static T easeOutElastic(T x) {
  const double c4 = (2.0 * glm::pi<T>()) / 3.0;
  return x == 0.0 ? 0.0 : x == 1 ? 1.0 : glm::pow(2.0, -10.0 * x) * glm::sin((x * 10.0 - 10.75) * c4) + 1.0;
}

template <typename T = float> static T easeInOutElastic(T x) {
  const double c5 = (2.0 * glm::pi<T>()) / 4.5;
  return x == 0.0  ? 0.0
         : x == 1  ? 1.0
         : x < 0.5 ? -(glm::pow(2.0, 20.0 * x - 10.0) * glm::sin((20.0 * x - 11.125) * c5)) / 2.0
                   : (glm::pow(2.0, -20.0 * x + 10.0) * glm::sin((20.0 * x - 11.125) * c5)) / 2.0 + 1.0;
}

template <typename T = float> static T easeOutBounce(T x) {
  const double n1 = 7.5625;
  const double d1 = 2.75;
  if (x < 1.0 / d1) {
    return n1 * x * x;
  } else if (x < 2.0 / d1) {
    return n1 * (x -= 1.5 / d1) * x + 0.75;
  } else if (x < 2.5 / d1) {
    return n1 * (x -= 2.25 / d1) * x + 0.9375;
  } else {
    return n1 * (x -= 2.265 / d1) * x + 0.984375;
  }
}

template <typename T = float> static T easeInBounce(T x) { return 1.0 - easeOutBounce(1.0 - x); }

template <typename T = float> static T easeInOutBounce(T x) {
  return x < 0.5 ? (1.0 - easeOutBounce(1.0 - 2.0 * x)) / 2.0 : (1.0 + easeOutBounce(2.0 * x - 1.0)) / 2.0;
}

} // namespace framework::easing