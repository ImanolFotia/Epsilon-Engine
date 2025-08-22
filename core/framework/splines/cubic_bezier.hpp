#pragma once

#include "spline.hpp"

namespace framework::splines
{ 
	template<typename T = glm::vec3>
	class CubicBezier : public Spline<T>
	{
	public:
		T getPoint(float t) {
			if (this->m_pControlPoints.size() < 4) return {};

			CurrentSegment<T> s = CalculateCurrentSegment(t);

			t = glm::fract(t);

			float t2 = t * t;
			float t3 = t * t * t;

			T q0 = (1.0 - t) * (1.0 - t) * (1.0 - t) * s.p0;//(s.p1 - s.p0) * (3.0f * t2);
			T q1 = 3.0 * t * (1.0 - t) * (1.0 - t) * s.p1;//(s.p2 - s.p1) * (6.0f * t * (1.0f - t));
			T q2 = 3.0 * t2 * (1.0 - t)  * s.p2;//(s.p3 - s.p2) * (3.0f * t2);
            T q3 = t3 * s.p3;
			return q0 + q1 + q2 + q3;
		}

		T getGradient(float t) {

			if (this->m_pControlPoints.size() < 4) return {};

			CurrentSegment s = CalculateCurrentSegment(t);

			t = glm::fract(t);
			float t2 = t * t;
			float t3 = t * t * t;
			
			T q0 = 3.0 * (1.0 - t) * (1.0 - t) * (s.p1 - s.p0);//(s.p1 - s.p0) * (3.0f * t2);
			T q1 = 6.0 * t * (1.0 - t) * (s.p2 - s.p1);//(s.p2 - s.p1) * (6.0f * t * (1.0f - t));
			T q2 = 3.0 * t2 * (s.p3 - s.p2);//(s.p3 - s.p2) * (3.0f * t2);

			return q0 + q1 + q2;
		}

		CurrentSegment<T> CalculateCurrentSegment(float t) {

			T p0, p1, p2, p3;

			if (this->m_pLoop) {
				p0 = this->m_pControlPoints.at((int)t >= 1 ? (int)t - 1 : this->m_pControlPoints.size() - 1).position;
				p1 = this->m_pControlPoints.at((int)t).position;
				p2 = this->m_pControlPoints.at(((int)t + 1) % this->m_pControlPoints.size()).position;
				p3 = this->m_pControlPoints.at(((int)t + 2) % this->m_pControlPoints.size()).position;
			}
			else {
				p0 = this->m_pControlPoints.at((int)t).position;
				p1 = this->m_pControlPoints.at(((int)t + 1)).position;
				p2 = this->m_pControlPoints.at(((int)t + 2)).position;
				p3 = this->m_pControlPoints.at(((int)t + 3)).position;
			}

			return { p0, p1, p2, p3 };
		}
	};
};