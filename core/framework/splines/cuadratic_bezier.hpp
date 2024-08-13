#pragma once

#include "spline.hpp"

namespace framework::splines
{
    class CuadraticBezier : public Spline
    {
    public:
        glm::vec3 getPoint(float t) {

            CurrentSegment s = CalculateCurrentSegment(t);
            
                glm::vec3 a_b = interpolate(s.p0, s.p1, t);
                glm::vec3 b_c = interpolate(s.p1, s.p2, t);

                return interpolate(a_b, b_c, t);
        

            //float t2 = t * t;

            //return -t2 * s.p0 + 2.0f * -t * t * s.p1 + t2 * s.p2;
        }

		glm::vec3 getGradient(float t) {

            CurrentSegment s = CalculateCurrentSegment(t);

            return  -2.0f * t * s.p0 + -2.0f * s.p1 + 2.0f * t * s.p2;
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



        CurrentSegment CalculateCurrentSegment(float t) {

            glm::vec3 p0, p1, p2, p3;

            if (m_pLoop) {
                p1 = m_pControlPoints.at((int)t % m_pControlPoints.size()).position;
                p2 = m_pControlPoints.at(((int)t + 1) % m_pControlPoints.size()).position;
                p3 = m_pControlPoints.at(((int)t + 2) % m_pControlPoints.size()).position;
            }
            else {
                p1 = m_pControlPoints.at(((int)t) % m_pControlPoints.size()).position;
                p2 = m_pControlPoints.at(((int)t + 1) % m_pControlPoints.size()).position;
                p3 = m_pControlPoints.at(((int)t + 2) % m_pControlPoints.size()).position;
            }

            return { p1, p2, p3 };
        }

    };
}