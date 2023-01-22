//
// Created by solaire on 12/14/22.
//

#ifndef LEARNINGVULKAN_QUAD_HPP
#define LEARNINGVULKAN_QUAD_HPP

#include "primitive.hpp"
namespace engine {
    class Quad : public PrimitiveBase {
    public:
        Quad() {

            m_pType = DrawableType::QUAD;
            using vtx = Vertex;
            using v4 = glm::vec4;
            using v3 = glm::vec3;
            using v2 = glm::vec2;
            m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f))); // bottom-left
            m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v2(1.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));  // bottom-right
            m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v2(1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));   // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v2(1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));   // top-right
            m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v2(0.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));  // top-left
            m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f))); // bottom-left

            for (int i = 0; i < m_pMesh.Vertices.size(); i += 3)
            {
                m_pMesh.addTriangle(i, i + 1, i + 2);
            }

            generateTangentSpaceVectors();
        }
    };
}

#endif //LEARNINGVULKAN_QUAD_HPP
