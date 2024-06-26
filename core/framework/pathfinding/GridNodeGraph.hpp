#pragma once

#include <glm/glm.hpp>

namespace framework {
class GridNodeGraph {
public:
  struct Node {
    size_t index;
    glm::vec3 position;
    bool visited = false;
    bool obstacle = false;
    float weight = 0.0;
    float localDistance = 10000000000000000.0f;
    float globalDistance = 10000000000000000.0f;
    Node *parent = nullptr;
    std::vector<Node *> neighbors;

    void AddNeighbor(Node *node) { neighbors.push_back(node); }
  };

public:
  GridNodeGraph() = default;
  GridNodeGraph(size_t w, size_t h) : m_pWidth{w}, m_pHeight{h} { clear(); }

  size_t Width() const { return m_pWidth; }

  size_t Height() const { return m_pHeight; }

  Node &getNode(size_t index) { return m_pNodes[index]; }

  std::vector<Node> &getNodes() { return m_pNodes; }

  void clear() {
    m_pNodes.clear();
    m_pNodes.resize(m_pWidth * m_pHeight);
    // m_pNodes.resize(m_pWidth * m_pHeight);
    for (int x = 0; x < m_pWidth; x++) {
      for (int y = 0; y < m_pHeight; y++) {
        // m_pNodes[y * m_pWidth + x].obstacle = false;
        m_pNodes[y * m_pWidth + x].parent = nullptr;
        m_pNodes[y * m_pWidth + x].visited = false;
        /*m_pNodes[y * m_pWidth + x].localDistance = 10000000000000000.0f;
        m_pNodes[y * m_pWidth + x].globalDistance = 10000000000000000.0f;
        m_pNodes[y * m_pWidth + x].obstacle = false;
        m_pNodes[y * m_pWidth + x].weight = 0.0;*/
      }
    }
  }

  void AddNeighbor(size_t index, Node *node) {
    m_pNodes[index].neighbors.push_back(node);
  }

private:
  std::vector<Node> m_pNodes;
  size_t m_pWidth;
  size_t m_pHeight;
};
} // namespace framework