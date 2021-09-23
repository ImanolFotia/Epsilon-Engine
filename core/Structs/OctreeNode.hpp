#pragma once

#include "Node.hpp"

namespace Epsilon::Structs
{
    template<typename T>
    class Octree;

    template <typename T>
    class OctreeNode
    {
        friend Octree;

        using OctreeNode_ptr = std::shared_ptr<OctreeNode<T>>;
        using DataType = std::shared_ptr<T>;

    public:
        OctreeNode() = default;

        OctreeNode_ptr Parent()
        {
            return mParent;
        }

        bool isParent()
        {
            return mParent == nullptr;
        }

        bool isLeaf()
        {
            bool leaf = false;
            for (unsigned i = 0; i < 8; i++)
                leaf |= mChildren[i] != nullptr;

            return leaf;
        }

        OctreeNode_ptr *Children()
        {
            return mChildren;
        }

        OctreeNode_ptr Children(uint8_t index)
        {
            if (index < 0 || index > 7)
                return nullptr;

            return mChildren[index];
        }

        bool Insert(uint8_t pos, OctreeNode_ptr node)
        {
            if (pos < 0 || pos > 7)
                return false;

            mChildren[pos] = node;
            return true;
        }

        template <typename... Args>
        bool Emplace(uint8_t pos, Args &&...args)
        {

            if (pos < 0 || pos > 7)
                return false;

            mChildren[pos] = std::make_shared<OctreeNode<T>>();
            mChildren[pos]->mData = std::make_shared<T>(args...);

            return true;
        }

        void Delete()
        {
            mData = nullptr;
            for (int i = 0; i < 8; i++)
            {
                if (mChildren[i] != nullptr)
                {
                    mChildren[i]->Delete();
                    mChildren[i] = nullptr;
                }
            }
        }

        DataType Data()
        {
            return mData;
        }

    private:
        OctreeNode_ptr mParent = nullptr;
        OctreeNode_ptr mChildren[8] = {nullptr};

        DataType mData = nullptr;
    };
}