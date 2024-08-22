#pragma once

#include "core/framework/containers/dynamic_array.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace engine
{

struct NodeBase {
    using type = void;

    int Index() { return index; }

    std::shared_ptr<NodeBase> Parent() { return parent; }

   protected:
    int index = 0;
    std::shared_ptr<NodeBase> parent;

    std::unordered_map<std::size_t, std::vector<std::shared_ptr<NodeBase>>>
        children;

    void clear() {
        for (auto &[type, ptrs] : children) {
            for (auto ptr : ptrs) {
                ptr->clear();
                ptr.reset();
            }
            ptrs.clear();
            ptrs.resize(0);
        }

        children.clear();
    }

    friend struct SceneManager;
};
    template <typename T>
    struct Node : NodeBase {
        template <class... Args>
        Node(Args&&... args) : data(std::forward<Args>(args)...) {}
        using type = T;
        T data;
    };

    struct Root{};

struct NodePosition {
    std::size_t type_id;
    std::size_t container_position;
};
}