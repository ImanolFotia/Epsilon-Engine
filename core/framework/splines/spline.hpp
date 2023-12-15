#pragma once

#include <glm/glm.hpp>

namespace framework::splines {

	struct ControlPoint {
		glm::vec3 position{};
		float length{};
	};

	struct CurrentSegment {
		glm::vec3 p0{};
		glm::vec3 p1{};
		glm::vec3 p2{};
		glm::vec3 p3{};
	};

	class Spline {
	public:
		void addPoint(glm::vec3 point) {
			m_pControlPoints.push_back({ .position = point, .length = 0 });
		}

		size_t numPoints() {
			return m_pControlPoints.size();
		}

		void Loops(bool val) {
			m_pLoop = val;
		}

		glm::vec3 getLastPoint() {
			return m_pControlPoints.back().position;
		}

		void Clear() {
			m_pControlPoints.clear();
		}
	protected:

		CurrentSegment CalculateCurrentSegment(float t) {

			glm::vec3 p0, p1, p2, p3;

			if (m_pLoop) {
				p0 = m_pControlPoints.at((int)t >= 1 ? (int)t - 1 : m_pControlPoints.size() - 1).position;
				p1 = m_pControlPoints.at((int)t).position;
				p2 = m_pControlPoints.at(((int)t + 1) % m_pControlPoints.size()).position;
				p3 = m_pControlPoints.at(((int)t + 2) % m_pControlPoints.size()).position;
			}
			else {
				p0 = m_pControlPoints.at((int)t).position;
				p1 = m_pControlPoints.at(((int)t + 1) % m_pControlPoints.size()).position;
				p2 = m_pControlPoints.at(((int)t + 2) % m_pControlPoints.size()).position;
				p3 = m_pControlPoints.at(((int)t + 3) % m_pControlPoints.size()).position;
			}

			return { p0, p1, p2, p3 };
		}

		std::vector<ControlPoint> m_pControlPoints{};
		bool m_pLoop = true;
	};
}