#pragma once

#include "spline.hpp"

namespace framework::splines {
	class Lagrange : public Spline {
	public:
		glm::vec3 Integrate(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float t)
		{
			return
				A *
				(
					(t - c_x1) / (c_x0 - c_x1) *
					(t - c_x2) / (c_x0 - c_x2) *
					(t - c_x3) / (c_x0 - c_x3)
					) +
				B *
				(
					(t - c_x0) / (c_x1 - c_x0) *
					(t - c_x2) / (c_x1 - c_x2) *
					(t - c_x3) / (c_x1 - c_x3)
					) +
				C *
				(
					(t - c_x0) / (c_x2 - c_x0) *
					(t - c_x1) / (c_x2 - c_x1) *
					(t - c_x3) / (c_x2 - c_x3)
					) +
				D *
				(
					(t - c_x0) / (c_x3 - c_x0) *
					(t - c_x1) / (c_x3 - c_x1) *
					(t - c_x2) / (c_x3 - c_x2)
					);
		}
	};
}