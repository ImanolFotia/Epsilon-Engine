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
} // namespace framework::easing