#pragma once

#include "spline.hpp"

namespace framework::splines
{
	class CubicBezier : public Spline
	{
	public:
		glm::vec3 getPoint(float t) {
			if (m_pControlPoints.size() < 4) return {};

			CurrentSegment s = CalculateCurrentSegment(t);

			t = glm::fract(t);

			float t2 = t * t;

			glm::vec3 q0 = (s.p1 - s.p0) * (3.0f * t2);
			glm::vec3 q1 = (s.p2 - s.p1) * (6.0f * t * (1.0f - t));
			glm::vec3 q2 = (s.p3 - s.p2) * (3.0f * t2);

			return q0 + q1 + q2;
		}

		glm::vec3 getGradient() {

			if (m_pControlPoints.size() < 4) return {};

			CurrentSegment s = CalculateCurrentSegment(t);

			t = glm::fract(t);

			return (s.p2 - 2.0f * s.p1 + s.p0) * (6.0f * (1.0f - t)) +
				   (s.p3 - 2.0f * s.p2 + s.p1) * (6.0f * t);
		}
	};
};