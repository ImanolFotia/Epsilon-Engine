#pragma once

#include <memory>
#include <vector>
#include <functional>

namespace engine
{

    struct NodeBase
    {
        using type = void;

    private:
        int index = 0;
        std::shared_ptr<NodeBase> parent;

        std::vector<std::function<void()>> destroy_children;

        friend class SceneManager;
    };

    template <typename T>
    struct Node : NodeBase
    {
        template <class... Args>
        Node(Args &&...args) : data(std::forward<Args>(args)...) {}
        using type = T;
        T data;
    };
    struct Root
    {
    };
}