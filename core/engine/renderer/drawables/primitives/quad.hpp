//
// Created by solaire on 12/14/22.
//

#ifndef LEARNINGVULKAN_QUAD_HPP
#define LEARNINGVULKAN_QUAD_HPP

#include "primitive.hpp"
namespace engine {
class Quad : public Primitive {

  struct {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
  } bounds;

  glm::vec2 m_pUvMultiplier = glm::vec2(1.0f);

public:
  enum BRUSH_TYPE { GAUSSIAN, SQUARE, SIZE };

  Quad(uint16_t tesselation = 2, glm::vec2 uvMultiplier = glm::vec2(1.0f), glm::vec3 size = glm::vec3(1.0f))
      : m_pTesselation(tesselation), m_pUvMultiplier(uvMultiplier) {

    m_pType = DrawableType::QUAD;

    if (tesselation <= 2) {
      pInit(size);
      return;
    }

    float step = 1.0f / ((float)tesselation + 1);
    int numVertices = (tesselation) * (tesselation);
    bounds.min = glm::vec3(100000.0f);
    bounds.max = glm::vec3(-100000.0f);

    glm::vec3 halfSize = size * 0.5f;

    for (int i = 0; i <= tesselation; i++) {
      for (int j = 0; j <= tesselation; j++) {
        float x = (float)i;
        float y = (float)j;
        v3 position = v3(x * step, 0.0f, y * step);
        v2 uv = v2(x, y) * uvMultiplier;
        v3 normal = v3(0.0f, 1.0f, 0.0f);

        position -= v3(0.5f, 0.0f, 0.5f);
        position *= size;

        if (position.x < bounds.min.x)
          bounds.min.x = position.x;
        if (position.x > bounds.max.x)
          bounds.max.x = position.x;

        if (position.z < bounds.min.z)
          bounds.min.z = position.z;
        if (position.z > bounds.max.z)
          bounds.max.z = position.z;

        m_pMesh.Vertices.emplace_back(vtx(position, uv, normal, v4(0.0f), v3(0.0f), v3(0.0f)));
      }
    }
    bounds.min.y = 0.0f;
    bounds.max.y = 1.0f;

    bounds.center = bounds.max - bounds.min;

    int edge = 0;
    for (int i = 0; i < numVertices + tesselation; i++) {
      if (edge >= tesselation) {
        edge = 0;
        continue;
      }
      m_pMesh.addTriangle(i, i + 1, i + tesselation + 1);
      m_pMesh.addTriangle(i + tesselation + 1, i + 1, i + tesselation + 2);
      edge++;
    }

    calculateNormals();
  }

  glm::vec3 getNormal(glm::vec3 position) {

    glm::ivec3 pos = glm::floor(position - 1.0f);

    int x_a = pos.x + m_pTesselation / 2;
    int x_b = 0;

    int y_a = pos.z + m_pTesselation / 2;
    int y_b = 0;

    if (x_a >= m_pTesselation) {
      x_b = x_a;
      x_a = x_a - 1;
    } else {
      x_b = x_a + 1;
    }

    if (y_a >= m_pTesselation) {
      y_b = y_a;
      y_a = y_a - 1;
    } else {
      y_b = y_a + 1;
    }

    if (y_a <= 0) {
      y_a = 0;
      y_b = 1;
    }

    if (x_a <= 0) {
      x_a = 0;
      x_b = 1;
    }

    glm::vec3 a, b, c, d;
    a = m_pMesh.Vertices.at(x_a * m_pTesselation + y_a).normal;
    b = m_pMesh.Vertices.at(x_b * m_pTesselation + y_a).normal;
    c = m_pMesh.Vertices.at(x_a * m_pTesselation + y_b).normal;
    d = m_pMesh.Vertices.at(x_b * m_pTesselation + y_b).normal;

    return glm::normalize(a);
  }

  void setHeight(float height, glm::vec3 position, float radius, BRUSH_TYPE brush) {

    glm::vec3 pos = glm::floor(position - glm::vec3(radius, 0.0f, radius) - 1.0f);

    glm::vec2 origin = glm::vec2(-128.0, -128.0);
    glm::vec2 bounds = glm::vec2(origin) + glm::vec2(m_pTesselation + 1, m_pTesselation + 1);

    glm::ivec2 p = glm::vec2(glm::ceil(pos.x - 0.5), glm::ceil(pos.z - 0.5));
    glm::ivec2 g = glm::vec2(glm::ceil(origin));

    p = (p - g);

    int index = p.y + p.x * (m_pTesselation + 1);

    if (position.x < origin.x || position.x >= bounds.x || position.z < origin.y || position.z >= bounds.y)
      index = 0;
    p += 2;
    for (int i = -1; i <= radius * 2; i++)
      for (int j = -1; j <= radius * 2; j++) {

        int index = (p.y + j) + (p.x + i) * (m_pTesselation + 1);
        auto &a = m_pMesh.Vertices.at(index);
        float t = glm::distance(position, a.position);
        if (t <= radius) {
          /*t = glm::clamp((1.5 - ((1.0 - t / radius) * 1.5)) * 2.0f, 0.0, 0.3);
          if (a.position.y < height) {
            a.position.y = glm::clamp(height - t, -10000.0f, height);
            a.position.y = glm::min(a.position.y, height);
          }*/
          /*if ((i == 0 || j == 0 || j + 1 == radius * 2 || i + 1 == radius * 2) && a.position.y < height) {
            a.position.y = height - 1.25f;
          } else if ((i == 1 || j == 1 || j + 2 == radius * 2 || i + 2 == radius * 2) && a.position.y < height) {
            a.position.y = height - 0.75f;
          } else {
          }*/

          a.position.y = height;

          // a.position.y += (height * (radius - t));

          // a.position.y = glm::clamp(a.position.y, -50.0f, height);
        }
      }
  }

  bool InitialPosition(glm::vec3 position, float radius, glm::ivec2 *output) {

    glm::vec3 pos = glm::floor(position - glm::vec3(radius, 0.0f, radius) - 1.0f);

    glm::vec2 origin = glm::vec2(-128.0, -128.0);
    glm::vec2 bounds = glm::vec2(origin) + glm::vec2(m_pTesselation + 1, m_pTesselation + 1);

    if (pos.x < origin.x || pos.x >= bounds.x || pos.z < origin.y || pos.z >= bounds.y)
      return false;
    glm::ivec2 p = glm::vec2(glm::ceil(pos.x - 0.5), glm::ceil(pos.z - 0.5));
    glm::ivec2 g = glm::vec2(glm::ceil(origin));

    *output = (p - g);

    return true;
  }

  void GaussianBrush(glm::vec3 position, float height, float radius) {

    glm::ivec2 p;
    if (!InitialPosition(position, radius, &p))
      return;

    for (int i = 0; i < radius * 2; i++)
      for (int j = 0; j < radius * 2; j++) {

        int index = (p.y + j) + (p.x + i) * (m_pTesselation + 1);
        auto &a = m_pMesh.Vertices.at(index);
        float t = glm::distance(position, a.position);
        if (t <= radius)
          a.position.y += (height * (radius - t));
      }
  }

  void SquareBrush(glm::vec3 position, float height, float radius) {

    glm::ivec2 p;
    if (!InitialPosition(position, radius, &p))
      return;
    p += 2;
    for (int i = 0; i < radius * 2; i++)
      for (int j = 0; j < radius * 2; j++) {

        int index = (p.y + j) + (p.x + i) * (m_pTesselation + 1);
        auto &a = m_pMesh.Vertices.at(index);
        a.position.y += height;
      }
  }

  void addHeight(float height, glm::vec3 position, float radius, BRUSH_TYPE brush) {

    glm::vec3 pos = glm::floor(position - glm::vec3(radius, 0.0f, radius) - 1.0f);

    glm::vec2 origin = glm::vec2(-128.0, -128.0);
    glm::vec2 bounds = glm::vec2(origin) + glm::vec2(m_pTesselation + 1, m_pTesselation + 1);

    glm::ivec2 p = glm::vec2(glm::ceil(pos.x - 0.5), glm::ceil(pos.z - 0.5));
    glm::ivec2 g = glm::vec2(glm::ceil(origin));

    p = (p - g);

    if (pos.x < origin.x || pos.x >= bounds.x || pos.z < origin.y || pos.z >= bounds.y)
      return;
    switch (brush) {
    case BRUSH_TYPE::GAUSSIAN:
      GaussianBrush(position, height, radius);
      break;
    case BRUSH_TYPE::SQUARE:
      SquareBrush(position, height, radius);
      break;
    default:
      break;
    }
  }

  float getHeight(glm::vec3 position, glm::vec3 scale) {
    glm::ivec3 pos = glm::floor(position - 1.0f);

    int x_a = pos.x + (m_pTesselation) / 2;
    int x_b = 0;

    int y_a = pos.z - (m_pTesselation) / 2;
    int y_b = 0;

    if (x_a >= m_pTesselation + 1) {
      x_b = x_a;
      x_a = x_a - 1;
    } else {
      x_b = x_a + 1;
    }

    if (y_a >= m_pTesselation + 1) {
      y_b = y_a;
      y_a = y_a - 1;
    } else {
      y_b = y_a + 1;
    }

    if (y_a < 0) {
      y_a = 0;
      y_b = 1;
    }

    if (x_a < 0) {
      x_a = 0;
      x_b = 1;
    }

    glm::vec2 m_pGridOrigin = glm::vec2(-128.0, -128.0);
    glm::vec2 bounds = glm::vec2(m_pGridOrigin) + glm::vec2(m_pTesselation + 1, m_pTesselation + 1);

    glm::ivec2 p = glm::vec2(glm::ceil(position.x - 0.5), glm::ceil(position.z - 0.5));
    glm::ivec2 g = glm::vec2(glm::ceil(m_pGridOrigin));

    p = (p - g);

    int index = p.y + p.x * (m_pTesselation + 1);

    if (position.x < m_pGridOrigin.x || position.x >= bounds.x || position.z < m_pGridOrigin.y || position.z >= bounds.y)
      index = 0;

    glm::vec3 a, b, c, d;
    a = m_pMesh.Vertices.at(index).position * scale; // a.x = a.x * scale.x; a.z = a.z * scale.z;
    b = a;
    c = a;
    d = a;

    if (index + (m_pTesselation + 1) < m_pMesh.Vertices.size())
      b = m_pMesh.Vertices.at(index + (m_pTesselation + 1)).position * scale; // b.x = b.x * scale.x; b.z = b.z * scale.z;
    if (index + 1 < m_pMesh.Vertices.size())
      c = m_pMesh.Vertices.at(index + 1).position * scale; // c.x = c.x * scale.x; c.z = c.z * scale.z;
    if (index + (m_pTesselation + 2) < m_pMesh.Vertices.size())
      d = m_pMesh.Vertices.at(index + (m_pTesselation + 2)).position * scale; // d.x = d.x * scale.x; d.z = d.z * scale.z;

    float la = glm::distance(position, a) / 1.4142;
    float lb = glm::distance(position, b) / 1.4142;
    float lc = glm::distance(position, c) / 1.4142;
    float ld = glm::distance(position, d) / 1.4142;

    float tx = lb - la;
    float ty = ld - lc;

    return ((a.y * tx + b.y * (1.0 - tx) + c.y * ty + d.y * (1.0 - ty)) * 0.5f);
  }

  void calculateNormals() {

    std::vector<std::vector<float>> Grid;
    std::vector<float> row;
    for (int i = 0; i < m_pTesselation; i++) {
      for (int j = 0; j < m_pTesselation; j++) {
        row.push_back(m_pMesh.Vertices.at(i * m_pTesselation + j).position.y);
      }
      Grid.push_back(row);
      row.clear();
    }

    std::vector<glm::vec3> Normals;
    /*for (int i = 0; i <= m_pTesselation; i++)
    {
            for (int j = 0; j <= m_pTesselation; j++)
            {*/
    int numVertices = m_pTesselation * m_pTesselation;
    for (int i = 0; i <= numVertices - m_pTesselation + 1; i++) {
      if (i > m_pTesselation + 1 && i < numVertices - m_pTesselation + 1) {
        // int index = i * m_pTesselation + j + 1;

        float HL = m_pMesh.Vertices.at(i - 1).position.y;
        float HR = m_pMesh.Vertices.at(i + 1).position.y;
        float HD = m_pMesh.Vertices.at(i - m_pTesselation).position.y;
        float HU = m_pMesh.Vertices.at(i + m_pTesselation).position.y;

        glm::vec3 norm = glm::normalize(glm::vec3((HL - HR), 1.0, (HD - HU)));

        m_pMesh.Vertices.at(i).normal = norm;
        // m_pMesh.Vertices.at(i * m_pTesselation + j).normal = norm;
      }
      //}
    }

    generateTangentSpaceVectors();
  }

private:
  void pInit(glm::vec3 size = glm::vec3(1.0f)) {
    m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 0.0f) * 0.5f * size, v2(1.0f, 1.0f) * m_pUvMultiplier, v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f),
                                      v3(0.0f, 1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f))); // bottom-left
    m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 0.0f) * 0.5f * size, v2(0.0f, 1.0f) * m_pUvMultiplier, v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f),
                                      v3(0.0f, 1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f))); // bottom-right
    m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 0.0f) * 0.5f * size, v2(0.0f, 0.0f) * m_pUvMultiplier, v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f),
                                      v3(0.0f, 1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f))); // top-right

    m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 0.0f) * 0.5f * size, v2(0.0f, 0.0f) * m_pUvMultiplier, v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f),
                                      v3(0.0f, 1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f))); // top-right
    m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 0.0f) * 0.5f * size, v2(1.0f, 0.0f) * m_pUvMultiplier, v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f),
                                      v3(0.0f, 1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f))); // top-left
    m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 0.0f) * 0.5f * size, v2(1.0f, 1.0f) * m_pUvMultiplier, v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f),
                                      v3(0.0f, 1.0f, 0.0f), v3(1.0f, 0.0f, 0.0f))); // bottom-left

    for (int i = 0; i < m_pMesh.Vertices.size(); i += 3) {
      m_pMesh.addTriangle(i, i + 1, i + 2);
    }

    // generateTangentSpaceVectors();
  }

  uint32_t m_pTesselation = 2;
};
} // namespace engine

#endif // LEARNINGVULKAN_QUAD_HPP
