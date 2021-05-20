#pragma once

#include <pch.hpp>
#include "Primitive.hpp"

namespace Epsilon
{

    template <typename T>
    class Cube : public PrimitiveBase
    {
    public:
        using Cube_ptr = std::shared_ptr<Cube<T>>;

        Cube(T w, T h, T l) : m_Width(w), m_Height(h), m_Length(l)
        {
            using vtx = Primitive::Vertex;
            using v3 = glm::vec3;
            using v2 = glm::vec2;
            mVertices = {
                // back face
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f)), // bottom-left
                vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 0.0f)),  // bottom-right
                vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f)), // bottom-left
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 1.0f)),  // top-left
                // front face
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f)), // bottom-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 0.0f)),  // bottom-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 1.0f)),  // top-left
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f)), // bottom-left
                // left face
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-right
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)),  // top-left
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-left
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-left
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)),  // bottom-right
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-right
                // right face
                vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-left
                vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-right
                vtx(v3(1.0f, 1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)),  // top-right
                vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)),  // bottom-left
                // bottom face
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f)), // top-right
                vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 1.0f)),  // top-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-left
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 0.0f)),  // bottom-right
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f)), // top-right
                // top face
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f)), // top-left
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-right
                vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 1.0f)),  // top-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-right
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f)), // top-left
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 0.0f))   // bottom-left
            };
        }

        void Render() override {}
        void Update() override {}
        void Destroy() override {}

        glm::vec3 getDimension()
        {
            return glm::vec3(m_Width, m_Height, m_Length);
        }

    private:
        T m_Width;
        T m_Height;
        T m_Length;
    };

} // namespace Epsilon