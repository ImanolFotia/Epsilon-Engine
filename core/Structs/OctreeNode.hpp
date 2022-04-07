#pragma once

#include "Node.hpp"
#include "AABB.hpp"
#include <Types.h>

namespace Epsilon::Structs
{
    template <typename T>
    class Octree;

    template <typename T>
    class OctreeNode
    {
        friend Octree;

        using OctreeNode_ptr = std::shared_ptr<OctreeNode<T>>;
        using DataType = std::shared_ptr<T>;

        enum CUADRANT_INDEX
        {
            TOP_RIGHT_FRONT = 0,
            TOP_RIGHT_BACK,
            TOP_LEFT_FRONT,
            TOP_LEFT_BACK,
            BOTTOM_RIGHT_FRONT,
            BOTTOM_RIGHT_BACK,
            BOTTOM_LEFT_FRONT,
            BOTTOM_LEFT_BACK
        };

        const glm::vec3 CUADRANT[] = {
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, -1.0),
            glm::vec3(-1.0, 1.0, 1.0),
            glm::vec3(-1.0, 1.0, -1.0),
            glm::vec3(1.0, -1.0, 1.0),
            glm::vec3(1.0, -1.0, -1.0),
            glm::vec3(-1.0, -1.0, 1.0),
            glm::vec3(-1.0, -1.0, -1.0)
        };


    public:
        OctreeNode() = default;

        OctreeNode(glm::vec3 c, glm::vec3 mmp) : m_Center(c), m_MinMaxPoints(mmp)
        {
            if(MIN_SIZE >= )
            init();
        }

        void parent(OctreeNode_ptr p)
        {
            m_Parent = p;
        }

        OctreeNode_ptr parent()
        {
            return m_Parent;
        }

        void init()
        {
            for (int i = 0; i < 8; i++)
            {
                glm::vec3 new_center = (m_MinMaxPoints / 4.0f) * CUADRANT[i];
                glm::vec3 new_mmp = (m_MinMaxPoints / 2.0f);
                m_Children[i] = std::make_shared<OctreeNode<T>>(new_center, new_mmp);
            }
        }

        bool isParent()
        {
            return m_Parent == nullptr;
        }

        bool isLeaf()
        {
            bool leaf = false;
            for (unsigned i = 0; i < 8; i++)
                leaf |= m_Children[i] != nullptr;

            return leaf;
        }

        OctreeNode_ptr *children()
        {
            return m_Children;
        }

        OctreeNode_ptr children(uint8_t index)
        {
            if (index < 0 || index > 7)
                return nullptr;

            return m_Children[index];
        }

        bool insert(glm::vec3 pos, OctreeNode_ptr node)
        {
            m_Children[pos] = node;
            return true;
        }

        template <typename... Args>
        bool emplace(uint8_t pos, Args &&...args)
        {

            if (pos < 0 || pos > 7)
                return false;

            m_Children[pos] = std::make_shared<OctreeNode<T>>();
            m_Children[pos]->m_Data = std::make_shared<T>(args...);

            return true;
        }

        bool intersect() {
            
            for (int i = 0; i < 8; i++)
            {
                AABB aabb = m_Children[i]->
            }
        }

        void destroy ()
        {
            m_Data = nullptr;
            for (int i = 0; i < 8; i++)
            {
                if (m_Children[i] != nullptr)
                {
                    m_Children[i]->delete ();
                    m_Children[i] = nullptr;
                }
            }
        }

        AABB getAABB() {

        }

        DataType data()
        {
            return mData;
        }

        const int MIN_SIZE = 1;

    public:
        OctreeNode_ptr m_Parent = nullptr;
        OctreeNode_ptr m_Children[8] = {nullptr};

        DataType m_Data = nullptr;

        glm::vec3 m_Center;
        glm::vec3 m_Size;
    };
}