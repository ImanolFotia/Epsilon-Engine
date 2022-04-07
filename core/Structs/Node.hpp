#pragma once

#include <pch.hpp>

namespace Epsilon::Structs
{
    class Node
    {
    public:
        using Node_ptr = std::shared_ptr<Node>;

    public:
        Node() = default;

        Node_ptr previous() {
            return m_Previous;
        }

        Node_ptr next() {
            return m_Next;
        }

    protected:
        Node_ptr m_Previous;
        Node_ptr m_Next;
        bool m_IsRoot;
    };

}