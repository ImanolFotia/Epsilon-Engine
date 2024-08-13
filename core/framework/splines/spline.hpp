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
			return m_pControlPoints.at(m_pControlPoints.size() - 2).position;
		}

		void Clear() {
			m_pControlPoints.clear();
		}

	protected:


		std::vector<ControlPoint> m_pControlPoints{};
		bool m_pLoop = true;
	};
}