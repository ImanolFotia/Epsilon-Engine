#pragma once

#include <unordered_map>
#include <vector>

#include "../object_pool.hpp"
#include "types.hpp"

namespace engine {

struct ResourceCache {

  struct Block {
    bool empty = true;
    size_t size = 0;
    size_t offset = 0;
    Ref<Buffer> buffer;
  };

  std::unordered_map<size_t, std::vector<Block>> FreeResources;

  void Free(Block block) { FreeResources[block.size].push_back(block); }

  Block GetBlock(size_t size) {
    for (auto &[key, resources] : FreeResources) {
      if (key >= size && resources.size() > 0) {
        Block block = resources.back();
        block.empty = false;
        resources.erase(resources.end() - 1);
        return block;
      }
    }

    return {};
  }
};
} // namespace engine