#pragma once

#include "spline.hpp"

namespace framework::splines {
	class CatmullRom : public Spline {
	public:

		glm::vec3 getPoint(float t) {

			if (m_pControlPoints.size() < 4) return {};
			glm::vec3 p0, p1, p2, p3;


			if (m_pLoop) {
				p0 = m_pControlPoints.at((int)t >= 1 ? (int)t - 1 : m_pControlPoints.size() - 1).position;
				p1 = m_pControlPoints.at((int)t).position;
				p2 = m_pControlPoints.at(((int)t + 1) % m_pControlPoints.size()).position;
				p3 = m_pControlPoints.at(((int)t + 2) % m_pControlPoints.size()).position;
			}
			else {
				p0 = m_pControlPoints.at((int)t).position;
				p1 = m_pControlPoints.at((int)t + 1).position;
				p2 = m_pControlPoints.at((int)t + 2).position;
				p3 = m_pControlPoints.at((int)t + 3).position;
			}
			t = glm::fract(t);

			float t2 = t * t;
			float t3 = t * t * t;

			float q0 = -t3 + 2.0f * t2 - t;
			float q1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
			float q2 = -3.0f * t3 + 4.0f * t2 + t;
			float q3 = t3 - t2;

			//return 0.5f * (glm::vec4(1.0f, t, t2, t3) * glm::mat4(glm::vec4(0.0f, 2.0f, 0.0f, 0.0f), glm::vec4(-1.0, 0.0, 1.0, 0.0), glm::vec4(2.0, -5.0, 4.0, -1.0), glm::vec4(-1.0, 3.0, -3.0, 1.0)) * glm::mat4(glm::vec4(p0, 0.0f), glm::vec4(p1, 0.0f), glm::vec4(p2, 0.0f), glm::vec4(p3, 0.0f)));

			return (p0 * q0 + p1 * q1 + p2 * q2 + p3 * q3) * 0.5f;
		}

		//derive the first function
		glm::vec3 getGradient(float t) {

			if (m_pControlPoints.size() < 4) return {};

			glm::vec3 p0, p1, p2, p3;

			if (m_pLoop) {
				p0 = m_pControlPoints.at((int)t >= 1 ? (int)t - 1 : m_pControlPoints.size() - 1).position;
				p1 = m_pControlPoints.at((int)t).position;
				p2 = m_pControlPoints.at(((int)t + 1) % m_pControlPoints.size()).position;
				p3 = m_pControlPoints.at(((int)t + 2) % m_pControlPoints.size()).position;
			}
			else {
				p0 = m_pControlPoints.at((int)t).position;
				p1 = m_pControlPoints.at((int)t + 1).position;
				p2 = m_pControlPoints.at((int)t + 2).position;
				p3 = m_pControlPoints.at((int)t + 3).position;
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
			glm::vec3 p1{}, p0 = getPoint(float(point));
			for (float x = 0.0f; x < 1.0f; x += 1.0f / 15.0f) {
				p1 = getPoint(float(point) + x);
				length += glm::distance(p1, p0);
				p0 = p1;
			}


		}

		glm::vec3 getLastPoint() {
			return m_pControlPoints.back().position;
		}

	};
}