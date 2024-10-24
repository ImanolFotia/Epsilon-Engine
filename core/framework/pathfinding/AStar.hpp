#pragma once

#include "GridNodeGraph.hpp"
#include <core/framework/common.hpp>

#include <limits>
#include <list>

#include <glm/gtx/hash.hpp>

namespace framework {

class AStar {

public:
  static float Heuristic(glm::vec3 a, glm::vec3 b) { return glm::distance(a, b); }

  static std::vector<GridNodeGraph::Node> traverse(GridNodeGraph &nodeGraph, size_t start_index, size_t end_index, bool ignore_breakable = false) {
    using Node = GridNodeGraph::Node;

    Node *start = &nodeGraph.getNode(start_index), *end = &nodeGraph.getNode(end_index);

    for (int i = 0; i < nodeGraph.Width() * nodeGraph.Height(); i++) {
      Node *node = &nodeGraph.getNode(i);
      node->visited = false;
      node->localDistance = std::numeric_limits<float>::max(); 
      node->globalDistance = std::numeric_limits<float>::max();
      node->parent = nullptr;
    }

    auto heuristic = [](Node *a, Node *b) -> float // So we can experiment with different heuristics
    { return glm::distance(a->position, b->position) + a->weight; };

    Node *currentNode = start;

    start->localDistance = 0.0f;
    start->globalDistance = heuristic(start, end);

    std::list<Node *> listNotTestedNodes;
    listNotTestedNodes.push_back(start);

    while (!listNotTestedNodes.empty() && currentNode != end) {
      listNotTestedNodes.sort([](const Node *lhs, const Node *rhs) { return lhs->globalDistance < rhs->globalDistance; });

      while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->visited)
        listNotTestedNodes.pop_front();

      if (listNotTestedNodes.empty())
        break;

      currentNode = listNotTestedNodes.front();
      currentNode->visited = true;

      for (auto nodeNeighbour : currentNode->neighbors) {
        if (!nodeNeighbour->visited && (!nodeNeighbour->obstacle || (nodeNeighbour->breakable && ignore_breakable))) {
          listNotTestedNodes.push_back(nodeNeighbour);
        }

        float fPossiblyLowerGoal = currentNode->localDistance + glm::distance(currentNode->position, nodeNeighbour->position);

        if (fPossiblyLowerGoal < nodeNeighbour->localDistance) {
          nodeNeighbour->parent = currentNode;
          nodeNeighbour->localDistance = fPossiblyLowerGoal;
          nodeNeighbour->globalDistance = nodeNeighbour->localDistance + heuristic(nodeNeighbour, end);
        }
      }
    }

    return ConstructPath(&nodeGraph.getNode(end->index), &nodeGraph.getNode(currentNode->index));
  }

  static std::vector<GridNodeGraph::Node> ConstructPath(GridNodeGraph::Node *startNode, GridNodeGraph::Node *endNode) {
    std::vector<GridNodeGraph::Node> path;
    path.clear();

    if(startNode == nullptr || endNode == nullptr) return {};

    GridNodeGraph::Node *currentNode = endNode;

    while (currentNode->parent != nullptr) {
      path.push_back(*currentNode);
      if(currentNode->parent == nullptr) break;
      currentNode = currentNode->parent;
    }

    if (endNode->index == startNode->index)
      Log::Info("end found!");

    return path;
  }
};

} // namespace framework