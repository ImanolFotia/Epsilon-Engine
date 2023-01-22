#pragma once

#include "glm/glm.hpp"

#include <vector>

#include "core/engine/renderer/drawables/drawable.hpp"

namespace engine
{

    class PrimitiveBase : public Drawable
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

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;

                glm::vec2 t0 = m_pMesh.Vertices[m_pMesh.Indices[i]].texCoords;
                glm::vec2 t1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].texCoords;
                glm::vec2 t2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].texCoords;

                glm::vec2 deltaUV1 = t1 - t0;
                glm::vec2 deltaUV2 = t2 - t0;

                // GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y + deltaUV2.x * deltaUV1.y);

                float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;
                // when t1, t2, t3 in same position in UV space, just use default UV direction.
                /*if (0 == deltaUV2.x && 0 == deltaUV2.y && 0 == deltaUV1.x && 0 == deltaUV1.y)
                {
                    deltaUV1.x = 0.0;
                    deltaUV1.y = 1.0;
                    deltaUV2.y = 1.0;
                    deltaUV2.y = 0.0;
                }*/

                tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                // tangent1 = glm::normalize(tangent1);

                m_pMesh.Vertices[m_pMesh.Indices[i]].tangent = tangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].tangent = tangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].tangent = tangent1;

                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                // bitangent1 = glm::normalize(bitangent1);

                m_pMesh.Vertices[m_pMesh.Indices[i]].bitangent = bitangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].bitangent = bitangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].bitangent = bitangent1;
            }
        }
    };

} // namespace Epsilon