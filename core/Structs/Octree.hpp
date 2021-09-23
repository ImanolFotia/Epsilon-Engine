#pragma once

#include "OctreeNode.hpp"

namespace Epsilon::Structs
{
    template <typename T>
    class Octree
    {
    public:
        Octree() = default;

        void Traverse()
        {
            /*...*/
        }

        void Serialize() { /*...*/ }
        
        void Deserialize() { /*...*/ }

    private:
        OctreeNode<T>::OctreeNode_ptr mRootNode;
    };
}