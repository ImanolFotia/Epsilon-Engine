#pragma once

#include "core/framework/pathfinding/GridNodeGraph.hpp"
#include <set>
#include <stack>

namespace framework {
class FloodFill {
  void Fill(GridNodeGraph &nodeGraph) {
    std::stack<int> stack;
    std::set<int> unique;
    int current_group = 1;

    int col = 255;

    auto &nodes = nodeGraph.getNodes();

    int num_nodes = nodes.size();

    for (int i = 0; i < num_nodes; i++) {
      if (unique.contains(i) || nodes[i].obstacle)
        continue;

      stack.push(i);
      current_group++;

      while (!stack.empty()) {
        int current = stack.top();
        stack.pop();

        if (current < 0 || current > num_nodes)
          continue;
        if (nodes[current].obstacle || nodes[current].visited)
          continue;
        if (unique.contains(current))
          continue;

        unique.insert(current);
        nodes[current].visited = true;
        nodes[current].node_group = current_group;

        //check all 8 neighbors
        if ((current + col) < num_nodes)
          stack.push(current + col);
        if ((current + col - 1) < num_nodes)
          stack.push(current + col - 1);
        if ((current + col + 1) < num_nodes)
          stack.push(current + col + 1);
        if ((current - col) > 0)
          stack.push(current - col);
        if ((current - col - 1) > 0)
          stack.push(current - col - 1);
        if ((current - col + 1) > 0)
          stack.push(current - col + 1);
        if ((current % col) - 1 >= 0)
          stack.push(current - 1);
        if ((current % col) + 1 < num_nodes)
          stack.push(current + 1);
      }
    }
  }
};
} // namespace framework