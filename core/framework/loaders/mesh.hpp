///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
/// Std. Includes
/// GL Includes
#include "eml1_0.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/common/common.hpp>
#include <string>

namespace framework {

class Mesh {
public:
  struct MeshData {
    size_t numVertices;
    size_t numIndices;
    size_t vertexOffset;
    size_t indexOffset;

    common::Mesh mesh;

  public:
    // MeshData() = default;
    // MeshData(auto a, auto b, auto c, auto d)
    //     : numVertices(a), numIndices(b), vertexOffset(c), indexOffset(d) {}
  };

  struct MeshMaterial {
    std::hash<std::string> id;
    std::string albedo;
    std::string roughness;
    std::string metallic;
    std::string normal;
    std::string ao;

    glm::vec4 color{};
    float roughness_color{};
    float metallic_color{};
    // float transmission = -1.0f;
    uint32_t mask;
  };

public:
  Mesh() = default;
  /**  Functions  */
  /// Constructor
  Mesh(std::vector<EML::t_Vertex> inVertices, std::vector<uint32_t> indices, MeshMaterial material, size_t vOffset,
       size_t iOffset) {
    std::vector<common::Vertex> vertices;

    auto Pos2Vec3 = [](EML::t_Position vtx) { return glm::vec3(vtx.x, vtx.y, vtx.z); };

    auto Tan2Vec3 = [](EML::t_Tangent vtx) { return glm::vec3(vtx.x, vtx.y, vtx.z); };

    auto Nor2Vec3 = [](EML::t_Normal vtx) { return glm::vec3(vtx.x, vtx.y, vtx.z); };

    auto Bi2Vec3 = [](EML::t_Bitangent vtx) { return glm::vec3(vtx.x, vtx.y, vtx.z); };

    auto Coord2Vec2 = [](EML::t_TexCoord vtx) { return glm::vec2(vtx.s, vtx.t); };

    for (auto &vtx : inVertices) {
      m_pData.mesh.Vertices.emplace_back(Pos2Vec3(vtx.position), Coord2Vec2(vtx.texcoord), Nor2Vec3(vtx.normal),
                                         glm::vec4(1.0), Tan2Vec3(vtx.tangent), Bi2Vec3(vtx.bitangent));
    }

    m_pData.mesh.Indices = indices;

    generateTangentSpaceVectors();

    m_pData = {.numVertices = m_pData.mesh.Vertices.size(),
               .numIndices = indices.size(),
               .vertexOffset = vOffset,
               .indexOffset = iOffset,
               .mesh = {m_pData.mesh.Vertices, m_pData.mesh.Indices}};

    m_pMaterial = material;
  }

  void setMinMaxPoints(auto minmax) { m_pMinMaxPoints = minmax; }

  auto getMinMaxPoints() { return m_pMinMaxPoints; }

  void setRelativePosition(auto position) { m_pRelativePosition = position; }

  MeshData &data() { return m_pData; }

  MeshMaterial &Material() { return m_pMaterial; }

  void generateTangentSpaceVectors() {
    uint32_t num_triangles = m_pData.mesh.Indices.size() / 3;
    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    // std::cout << m_pMesh.m_Tris.size() <<  std::endl;
    for (int i = 0; i < m_pData.mesh.Indices.size(); i += 3) {
      glm::vec3 v0 = m_pData.mesh.Vertices[m_pData.mesh.Indices[i]].position;
      glm::vec3 v1 = m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 1]].position;
      glm::vec3 v2 = m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 2]].position;

      glm::vec3 edge1 = v1 - v0;
      glm::vec3 edge2 = v2 - v0;

      glm::vec2 t0 = m_pData.mesh.Vertices[m_pData.mesh.Indices[i]].texCoords;
      glm::vec2 t1 = m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 1]].texCoords;
      glm::vec2 t2 = m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 2]].texCoords;

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

      m_pData.mesh.Vertices[m_pData.mesh.Indices[i]].tangent = tangent1;
      m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 1]].tangent = tangent1;
      m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 2]].tangent = tangent1;

      bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
      bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
      bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
      // bitangent1 = glm::normalize(bitangent1);

      m_pData.mesh.Vertices[m_pData.mesh.Indices[i]].bitangent = bitangent1;
      m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 1]].bitangent = bitangent1;
      m_pData.mesh.Vertices[m_pData.mesh.Indices[i + 2]].bitangent = bitangent1;
    }
  }

  void setName(std::string name) { m_pName = name; }

private:
  std::string m_pName{};
  glm::mat4 m_pLocalTransform{};
  glm::vec3 m_pRelativePosition{};
  common::MIN_MAX_POINTS m_pMinMaxPoints;

  MeshMaterial m_pMaterial;

  MeshData m_pData;
};
} // namespace framework