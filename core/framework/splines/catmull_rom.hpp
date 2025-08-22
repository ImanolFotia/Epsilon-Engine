#pragma once

#include "spline.hpp"

namespace framework::splines {

template <typename T = glm::vec3> class CatmullRom : public Spline<T> {
public:

  bool m_pEndOfPath = false;

  bool EndReached() { return m_pEndOfPath; }

  void RestartPath() { m_pEndOfPath = false; }

  T getPoint(float t) {

    if (this->m_pControlPoints.size() < 4)
      return {};

    if (!this->m_pLoop && t >= (float)this->m_pControlPoints.size() - 3) {
      m_pEndOfPath = true;
      t = this->m_pControlPoints.size() - 3;
    }

    CurrentSegment<T> s = CalculateCurrentSegment(t);

    t = glm::fract(t);

    float t2 = t * t;
    float t3 = t * t * t;

    float q0 = -t3 + 2.0f * t2 - t;
    float q1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
    float q2 = -3.0f * t3 + 4.0f * t2 + t;
    float q3 = t3 - t2;

    return (s.p0 * q0 + s.p1 * q1 + s.p2 * q2 + s.p3 * q3) * 0.5f;
  }

  // derive the first function
  T getGradient(float t) {

    if (this->m_pControlPoints.size() < 4)
      return {};

    T p0, p1, p2, p3;

    if (!this->m_pLoop && t >= (float)this->m_pControlPoints.size() - 3.0) {
      m_pEndOfPath = true;
      // return m_pControlPoints.at(m_pControlPoints.size()-2).position;
      t = this->m_pControlPoints.size() - 3;
    }

    if (this->m_pLoop) {
      p0 = this->m_pControlPoints.at((int)t >= 1 ? ((int)t - 1)% this->m_pControlPoints.size() : this->m_pControlPoints.size() - 1).position;
      p1 = this->m_pControlPoints.at((int)t % this->m_pControlPoints.size()).position;
      p2 = this->m_pControlPoints.at(((int)t + 1) % this->m_pControlPoints.size()).position;
      p3 = this->m_pControlPoints.at(((int)t + 2) % this->m_pControlPoints.size()).position;
    } else {
      p0 = this->m_pControlPoints.at((int)t).position;
      p1 = this->m_pControlPoints.at(((int)t + 1) % this->m_pControlPoints.size()).position;
      p2 = this->m_pControlPoints.at(((int)t + 2) % this->m_pControlPoints.size()).position;
      p3 = this->m_pControlPoints.at(((int)t + 3) % this->m_pControlPoints.size()).position;
    }

    t = glm::fract(t);

    float t2 = t * t;
    float t3 = t * t * t;

    float q0 = -3.0f * t2 + 4.0f * t - 1;
    float q1 = 9.0f * t2 - 10.0f * t;
    float q2 = -9.0f * t2 + 8.0 * t + 1;
    float q3 = 3.0 * t2 - 2.0 * t;

    return (p0 * q0 + p1 * q1 + p2 * q2 + p3 * q3) * 0.5f;
  }

  void CalculateLength(int point) {
    float length = 0.0f;
    T p1{}, p0 = getPoint(float(point));
    for (float x = 0.0f; x < 1.0f; x += 1.0f / 15.0f) {
      p1 = getPoint(float(point) + x);
      length += glm::distance(p1, p0);
      p0 = p1;
    }
  }

  CurrentSegment<T> CalculateCurrentSegment(float t) {

    T p0, p1, p2, p3;

    if (this->m_pLoop) {
      p0 = this->m_pControlPoints.at((int)t >= 1 ? (int)t - 1 : this->m_pControlPoints.size() - 1).position;
      p1 = this->m_pControlPoints.at((int)t% this->m_pControlPoints.size()).position;
      p2 = this->m_pControlPoints.at(((int)t + 1) % this->m_pControlPoints.size()).position;
      p3 = this->m_pControlPoints.at(((int)t + 2) % this->m_pControlPoints.size()).position;
    } else {
      p0 = this->m_pControlPoints.at((int)t).position;
      p1 = this->m_pControlPoints.at(((int)t + 1) % this->m_pControlPoints.size()).position;
      p2 = this->m_pControlPoints.at(((int)t + 2) % this->m_pControlPoints.size()).position;
      p3 = this->m_pControlPoints.at(((int)t + 3) % this->m_pControlPoints.size()).position;
    }

    return {p0, p1, p2, p3};
  }

  float CalculateLength() {
    size_t numPoints = this->m_pControlPoints.size() - 3;

    m_pIntervalDistances.resize(numPoints);

    float dist = 0.0f;

    for (int i = 0; i < numPoints; i++) {
      m_pIntervalDistances[i] = glm::distance(this->m_pControlPoints[i + 1].position, this->m_pControlPoints[i + 2].position);
    }

    for (int i = 0; i < numPoints; i++) {
      int divisions = (int)m_pIntervalDistances[i] * 10;
      float step = 1.0 / divisions;
      float t = i;

      for (int j = 0; j < divisions; j++) {
        auto a = getPoint(t);
        auto b = getPoint(t + step);
        dist += glm::distance(a, b);
        t += step;
      }
    }

    m_pEndOfPath = false;

    return dist;
  }

  float getIntervalDistance(int index) {
    if (index < m_pIntervalDistances.size())
      return m_pIntervalDistances[index];
    return 0.0f;
  }

  std::vector<float> m_pIntervalDistances;
};
} // namespace framework::splines