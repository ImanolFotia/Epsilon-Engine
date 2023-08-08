#pragma once

#include "glm/glm.hpp"

#include <vector>

#include "../drawable.hpp"

namespace engine
{

    class Primitive : public Drawable
    {
    public:

        void generateTangentSpaceVectors()
        {
            uint32_t num_triangles = m_pMesh.Indices.size() / 3;
            // calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent1, bitangent1;
            // std::cout << m_pMesh.m_Tris.size() <<  std::endl;
            for (int i = 0; i < m_pMesh.Indices.size(); i += 3)
            {
                glm::vec3 v0 = m_pMesh.Vertices[m_pMesh.Indices[i]].position;
                glm::vec3 v1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].position;
                glm::vec3 v2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].position;

                glm::vec3 edge1 = v0 - v1;
                glm::vec3 edge2 = v0 - v2;

                glm::vec2 t0 = m_pMesh.Vertices[m_pMesh.Indices[i]].texCoords;
                glm::vec2 t1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].texCoords;
                glm::vec2 t2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].texCoords;

                glm::vec2 deltaUV1 = t0 - t1;
                glm::vec2 deltaUV2 = t0 - t2;


                float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;


                tangent1 = glm::cross(m_pMesh.Vertices[m_pMesh.Indices[i]].normal, edge1);
                bitangent1 = glm::cross(m_pMesh.Vertices[m_pMesh.Indices[i]].normal, tangent1);
                

                m_pMesh.Vertices[m_pMesh.Indices[i]].tangent = glm::normalize(tangent1);
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].tangent = glm::normalize(tangent1);
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].tangent = glm::normalize(tangent1);

                // bitangent1 = glm::normalize(bitangent1);

                m_pMesh.Vertices[m_pMesh.Indices[i]].bitangent = glm::normalize(bitangent1);
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].bitangent = glm::normalize(bitangent1);
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].bitangent = glm::normalize(bitangent1);
            }
        }


    };

} // namespace Epsilon