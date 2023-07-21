#pragma once

#include "spline.hpp"

namespace framework::splines {
	class CatmullRom : public Spline {
    public:
        glm::vec3 Integrate(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float t)
        {
            float t2 = t * t;
            float t3 = t * t * t;
            glm::vec3 a = -A / 2.0 + (3.0 * B) / 2.0 - (3.0 * C) / 2.0 + D / 2.0;
            glm::vec3 b = A - (5.0 * B) / 2.0 + 2.0 * C - D / 2.0;
            glm::vec3 c = -A / 2.0 + C / 2.0;
            glm::vec3 d = B;

            return a * t3 + b * t2 + c * t + d;
        }
	};
}