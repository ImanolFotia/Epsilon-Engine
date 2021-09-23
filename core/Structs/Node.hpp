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

        Node_ptr Previous() {
            return mPrevious;
        }

        Node_ptr Next() {
            return mNext;
        }

    protected:
        Node_ptr mPrevious;
        Node_ptr mNext;
        bool mIsRoot;
    };

}