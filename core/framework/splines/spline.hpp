#pragma once

#include <glm/glm.hpp>

namespace framework::splines {

	struct ControlPoint {
		glm::vec3 position{};
		float length{};
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

	protected:
		std::vector<ControlPoint> m_pControlPoints{};
		bool m_pLoop = true;
	};
}