#pragma once

#include "primitive.hpp"

namespace engine
{

    class Cube : public Primitive
    {
        float mUVMultiplier = 1.0f;
        glm::vec3 m_pScale = glm::vec3(1.0f);
        glm::vec4 m_pColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    public:
        Cube()
        {
            pInit();
        }

        Cube(glm::vec3 scale) : m_pScale(scale){
            pInit();
        }

        Cube(glm::vec4 color) : m_pColor(color)
        {
            pInit();
        }

        void pInit() {
            m_pType = DrawableType::CUBE;
            using vtx = Vertex;
            using v4 = glm::vec4;
            using v3 = glm::vec3;
            using v2 = glm::vec2;
            // m_pMesh.Vertices = {
            //  back face
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 0.0f), v3(0.0f, 0.0f, -1.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, -0.5f) * m_pScale, v2(1.0f, 1.0f), v3(0.0f, 0.0f, -1.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, -0.5f) * m_pScale, v2(1.0f, 0.0f), v3(0.0f, 0.0f, -1.0f), m_pColor, v3(0.0f), v3(0.0f)));  // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, -0.5f) * m_pScale, v2(1.0f, 1.0f), v3(0.0f, 0.0f, -1.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 0.0f), v3(0.0f, 0.0f, -1.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(0.0f, 0.0f, -1.0f), m_pColor, v3(0.0f), v3(0.0f)));  // top-left

            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, 0.5f) * m_pScale, v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), m_pColor, v3(0.0f), v3(0.0f)));  // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, 0.5f) * m_pScale, v2(0.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), m_pColor, v3(0.0f), v3(0.0f)));  // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, 0.5f) * m_pScale, v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-left

            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(-1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, -0.5f) * m_pScale, v2(1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, 0.5f) * m_pScale, v2(0.0f, 0.0f), v3(-1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(-1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-right

            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, -0.5f) * m_pScale, v2(1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, 0.5f) * m_pScale, v2(0.0f, 0.0f), v3(1.0f, 0.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // bottom-left

            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, -0.5f) * m_pScale, v2(1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(0.0f, -1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, -0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(0.0f, -1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, 0.5f) * m_pScale, v2(0.0f, 0.0f), v3(0.0f, -1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, -0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // top-right

            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(0.0f, 1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, -0.5f) * m_pScale, v2(1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(0.5f, 0.5f, 0.5f) * m_pScale, v2(1.0f, 0.0f), v3(0.0f, 1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));   // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, -0.5f) * m_pScale, v2(0.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f))); // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-0.5f, 0.5f, 0.5f) * m_pScale, v2(0.0f, 0.0f), v3(0.0f, 1.0f, 0.0f), m_pColor, v3(0.0f), v3(0.0f)));  // bottom-left
            //};
            // Generate tangent space vectors

            for (int i = 0; i < m_pMesh.Vertices.size(); i += 3)
            {
                m_pMesh.addTriangle(i, i + 1, i + 2);
            }

            generateTangentSpaceVectors();
        }

    };

} // namespace Epsilon