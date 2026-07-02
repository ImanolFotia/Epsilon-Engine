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

		float getT(float d) {
    		if (this->m_pControlPoints.size() <= 3) return 0;
			
			if(d >= m_pDistanceMap.back().first) return m_pDistanceMap.back().second;

			if(d == 0) return 0;

			for(size_t i = 0; i < m_pDistanceMap.size()-1; i++) {
				auto & d0 = m_pDistanceMap[i];
				auto & d1 = m_pDistanceMap[i+1];
				if(d0.first <= d && d <= d1.first) {
					if(d1.second - d0.second > 1) return d0.second;
					return glm::lerp(d0.second, d1.second, (d1.first - d)/(d1.first-d0.first));
				} 
			}
			return 0.0f;
		}

	        virtual void CalculateDistance() = 0;

	        float Length() const { return length;}

	protected:

		std::vector<ControlPoint<T>> m_pControlPoints;
		std::vector<std::pair<float, float>> m_pDistanceMap;

		bool m_pLoop = true;
	        float length = 0.0f;
	};
}