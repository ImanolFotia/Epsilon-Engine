#pragma once

#include <glm/glm.hpp>

namespace framework {
class GridNodeGraph {
public:
  struct Node {
    size_t index{};
    glm::vec3 position{};
    bool visited = false;
    bool obstacle = false;
    bool breakable = false;
    float weight = 0.0;
    float localDistance = 10000000000000000.0f;
    float globalDistance = 10000000000000000.0f;
    Node *parent = nullptr;
    std::vector<Node *> neighbors{};
    int8_t level = 0;
    bool terrain_node = false;

    void AddNeighbor(Node *node) { neighbors.push_back(node); }
  };

public:
  GridNodeGraph() = default;
  GridNodeGraph(size_t w, size_t h) : m_pWidth{w}, m_pHeight{h} { m_pNodes.resize(m_pWidth * m_pHeight); }

  size_t Width() const { return m_pWidth; }

  size_t Height() const { return m_pHeight; }

  Node &getNode(size_t index) { return m_pNodes[index]; }

  std::vector<Node> &getNodes() { return m_pNodes; }

  void clear() {
    // m_pNodes.clear();
    if (m_pNodes.size() == 0)
      m_pNodes.resize(m_pWidth * m_pHeight);
    // m_pNodes.resize(m_pWidth * m_pHeight);
    for (int y = 0; y < m_pHeight; y++) {
      for (int x = 0; x < m_pWidth; x++) {
        // m_pNodes[y * m_pWidth + x].obstacle = false;
        int level = m_pNodes[y * m_pWidth + x].level;
        float py = m_pNodes[y * m_pWidth + x].position.y;
        bool obstacle = m_pNodes[y * m_pWidth + x].obstacle;
        bool breakable = m_pNodes[y * m_pWidth + x].breakable;
        bool terrain_node = m_pNodes[y * m_pWidth + x].terrain_node;
        m_pNodes[y * m_pWidth + x] = {};
        m_pNodes[y * m_pWidth + x].parent = nullptr;
        m_pNodes[y * m_pWidth + x].visited = false;
        m_pNodes[y * m_pWidth + x].level = level;
        m_pNodes[y * m_pWidth + x].obstacle = obstacle;
        m_pNodes[y * m_pWidth + x].breakable = breakable;
        m_pNodes[y * m_pWidth + x].terrain_node = terrain_node;
        m_pNodes[y * m_pWidth + x].position.y = py;
        /*m_pNodes[y * m_pWidth + x].localDistance = 10000000000000000.0f;
        m_pNodes[y * m_pWidth + x].globalDistance = 10000000000000000.0f;
        m_pNodes[y * m_pWidth + x].obstacle = false;
        m_pNodes[y * m_pWidth + x].weight = 0.0;*/
      }
    }
  }

  void AddNeighbor(size_t index, Node *node) { m_pNodes[index].neighbors.push_back(node); }

private:
  std::vector<Node> m_pNodes{};
  size_t m_pWidth{};
  size_t m_pHeight{};
};
} // namespace framework
