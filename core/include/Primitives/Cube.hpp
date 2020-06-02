#pragma once

#include "Primitive.hpp"

namespace Epsilon
{

    class Cube : public Primitive
    {
    public:

        Cube(double w, double h, double l) : m_Width(w), m_Height(h), m_Length(l) {

        }

        void Render() override {}
        void Update() override {}
        void Destroy() override {}

        glm::vec3 getDimension() {
            return glm::vec3(m_Width, m_Height, m_Length);
        }

        private:
            double m_Width;
            double m_Height;
            double m_Length;
    };

} // namespace Epsilon