#pragma once

#include <glm/glm.hpp>

namespace framework::splines {

	template<typename T = glm::vec3>
	struct ControlPoint {
		T position{};
		float length{};
	};

	template<typename T = glm::vec3>
	struct CurrentSegment {
		T p0{};
		T p1{};
		T p2{};
		T p3{};
	};
    
	template<typename T = glm::vec3>
	class Spline {
	public:
	    Spline() = default;

		void addPoint(T point) {
			m_pControlPoints.push_back({ .position = point, .length = 0 });
		}

		size_t numPoints() {
			return m_pControlPoints.size();
		}

		void Loops(bool val) {
			m_pLoop = val;
		}

		T getLastPoint() {
			return m_pControlPoints.at(m_pControlPoints.size() - 2).position;
		}

		void Clear() {
			m_pControlPoints.clear();
		}

		auto &getPoints () {
			return m_pControlPoints;
		}

	protected:

		std::vector<ControlPoint<T>> m_pControlPoints;
		bool m_pLoop = true;
	};
}