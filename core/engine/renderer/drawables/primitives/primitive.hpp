#pragma once

#include "glm/glm.hpp"

#include <vector>

#include "../drawable.hpp"

namespace engine {

class Primitive : public Drawable {
protected:
  using vtx = common::Vertex;
  using v4 = glm::vec4;
  using v3 = glm::vec3;
  using v2 = glm::vec2;

public:
  void generateTangentSpaceVectors() {
    uint32_t num_triangles = m_pMesh.Indices.size() / 3;
    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    // std::cout << m_pMesh.m_Tris.size() <<  std::endl;
    for (int i = 0; i < m_pMesh.Indices.size(); i += 3) {
      glm::vec3 v0 = m_pMesh.Vertices[m_pMesh.Indices[i]].position;
      glm::vec3 v1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].position;
      glm::vec3 v2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].position;

      if (v0 == v1)
        v0 = m_pMesh.Vertices[m_pMesh.Indices[i + 3]].position;

      glm::vec3 edge1 = v1 - v0;
      glm::vec3 edge2 = v2 - v0;

      glm::vec2 t0 = m_pMesh.Vertices[m_pMesh.Indices[i]].texCoords;
      glm::vec2 t1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].texCoords;
      glm::vec2 t2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].texCoords;

      glm::vec2 deltaUV1 = t1 - t0;
      glm::vec2 deltaUV2 = t2 - t0;

      float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;

      // tangent vector and binormal vector
      glm::vec3 t = deltaUV2.y * edge1 - deltaUV1.y * edge2;
      glm::vec3 b = deltaUV1.x * edge2 - deltaUV2.x * edge1;

      t = glm::cross(glm::cross(m_pMesh.Vertices[m_pMesh.Indices[i]].normal, t), t); // orthonormalization of the tangent vector
      b = glm::cross(b, glm::cross(b, m_pMesh.Vertices[m_pMesh.Indices[i]].normal)); // orthonormalization of the binormal vectors to the normal vector
      b = glm::cross(glm::cross(t, b), t);

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

} // namespace engine