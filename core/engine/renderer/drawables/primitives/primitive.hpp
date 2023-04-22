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

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;

                glm::vec2 t0 = m_pMesh.Vertices[m_pMesh.Indices[i]].texCoords;
                glm::vec2 t1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].texCoords;
                glm::vec2 t2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].texCoords;

                glm::vec2 deltaUV1 = t1 - t0;
                glm::vec2 deltaUV2 = t2 - t0;


                float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;
                

                tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

                m_pMesh.Vertices[m_pMesh.Indices[i]].tangent = tangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].tangent = tangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].tangent = tangent1;

                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

                m_pMesh.Vertices[m_pMesh.Indices[i]].bitangent = bitangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].bitangent = bitangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].bitangent = bitangent1;
            }
        }


    };

} // namespace Epsilon