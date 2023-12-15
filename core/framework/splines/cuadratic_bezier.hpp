#pragma once

#include "Bezier.hpp"

namespace framework::splines
{
    class CuadraticBezier : public Spline
    {
    public:
        glm::vec3 getPoint(float t) {
                vec3 a_b = lerp(P0, P1, t);
                vec3 b_c = lerp(P1, P2, t);

                return lerp(a_b, b_c, t);
        }

		glm::vec3 getGradient(float t) {

		}

        glm::vec3 lerp(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, float t)
        {
            glm::vec3 a_b = interpolate(P0, P1, t);
            glm::vec3 b_c = interpolate(P1, P2, t);

            return interpolate(a_b, b_c, t);
        }

        glm::vec3 interpolate(glm::vec3 a, glm::vec3 b, float t)
        {
            return (1.0f - t) * a + t * b;
        }

    };
}