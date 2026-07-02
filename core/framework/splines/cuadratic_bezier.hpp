#pragma once

#include "spline.hpp"

namespace framework::splines {
template <typename T = glm::vec3> class CuadraticBezier : public Spline<T> {
public:
  T getPoint(float t) {

    CurrentSegment<T> s = CalculateCurrentSegment(t);

    t = glm::fract(t);
    /*
    T a_b = interpolate(s.p0, s.p1, t);
    T b_c = interpolate(s.p1, s.p2, t);

    return interpolate(a_b, b_c, t);*/

    float t2 = t * t;

    return (1.0 - t) * (1.0 - t) * s.p0 + 2.0f * (1.0 - t) * t * s.p1 + t2 * s.p2;
  }

  T getGradient(float t) {

    CurrentSegment<T> s = CalculateCurrentSegment(t);
    t = glm::fract(t);

    return 2.0f * (1.0 - t) * (s.p1 - s.p0) + 2.0f * t * (s.p2 - s.p1);
  }

  T lerp(T P0, T P1, T P2, float t) {
    T a_b = interpolate(P0, P1, t);
    T b_c = interpolate(P1, P2, t);

    return interpolate(a_b, b_c, t);
  }

  T interpolate(T a, T b, float t) { return (1.0f - t) * a + t * b; }

  CurrentSegment<T> CalculateCurrentSegment(float t) {

    T p0, p1, p2, p3;

    if (this->m_pLoop) {
      p1 = this->m_pControlPoints.at((int)t % this->m_pControlPoints.size()).position;
      p2 = this->m_pControlPoints.at(((int)t + 1) % this->m_pControlPoints.size()).position;
      p3 = this->m_pControlPoints.at(((int)t + 2) % this->m_pControlPoints.size()).position;
    } else {
      t = glm::clamp(t, 0.0f, (float)this->m_pControlPoints.size() - 3);
      p1 = this->m_pControlPoints.at(((int)t)).position;
      p2 = this->m_pControlPoints.at(((int)t + 1)).position;
      p3 = this->m_pControlPoints.at(((int)t + 2)).position;
    }

    return {p1, p2, p3};
  }

  void CalculateDistance() override {

  }
};
} // namespace framework::splines