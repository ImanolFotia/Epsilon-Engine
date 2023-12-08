#pragma once

#include "Bezier.hpp"

namespace framework::splines
{
    class CuadraticBezier : public Bezier<3>
    {
    public:
        CuadraticBezier() : Bezier<3>() {
            type = DrawableType::CUADRATIC_BEZIER;
        }

        virtual void Update() override {}

        virtual void Render() override {}

        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) override {}

    private:
        glm::vec3 P_lerp(ControlPoint P0, ControlPoint P1, ControlPoint P2, float t)
        {
            glm::vec3 a_b = interpolate(P0.position, P1.position, t);
            glm::vec3 b_c = interpolate(P1.position, P2.position, t);

            return interpolate(a_b, b_c, t);
        }

        glm::vec3 interpolate(glm::vec3 a, glm::vec3 b, float t)
        {
            return (1.0f - t) * a + t * b;
        }
    };
}