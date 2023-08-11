#pragma once

#include <glm/glm.hpp>

namespace framework::splines {
	class Spline {
	public:
		void addPoint(glm::vec3 point) {
			m_pControlPoints.push_back(point);
		}

		size_t numPoints() {
			return m_pControlPoints.size();
		}

	protected:
		std::vector<glm::vec3> m_pControlPoints{};
		bool m_pLoop = true;
	};
}