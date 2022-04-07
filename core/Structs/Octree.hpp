#pragma once

#include <iostream>
#include <vector>

namespace Epsilon
{
    

    struct MIN_MAX_POINTS
    {
        glm::vec3 min;
        glm::vec3 max;
    };

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

    const glm::vec3 CUADRANT[] = {glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, -1.0),
                               glm::vec3(-1.0, 1.0, 1.0), glm::vec3(-1.0, 1.0, -1.0),
                               glm::vec3(1.0, -1.0, 1.0), glm::vec3(1.0, -1.0, -1.0),
                               glm::vec3(-1.0, -1.0, 1.0), glm::vec3(-1.0, -1.0, -1.0)};

    const int MAX_SIZE = 1;
    const int MAX_DEPTH = 5;

    template <typename T>
    class Octree
    {
        glm::vec3 m_Position;
        MIN_MAX_POINTS m_MinMaxPoints;

        bool m_IsBuilt = false;
        bool m_IsLeaf = false;

        bool m_IsEmpty = true;

        int m_Depth = 0;

        Octree<T> *m_Children[8];

        std::vector<T> m_Data = {};

    public:
        Octree(glm::vec3 position, MIN_MAX_POINTS minmax, int depth = 0)
            : m_Position(position), m_MinMaxPoints(minmax), m_Depth(depth)
        {
            glm::vec3 node_size = m_MinMaxPoints.max - m_MinMaxPoints.min;

            if (m_Depth < MAX_DEPTH)
                pBuild();
            else
                m_IsLeaf = true;
            m_IsBuilt = true;
        }

        void traverse()
        {
            for (auto &d : m_Data)
                std::cout << d << std::endl;
            if (m_IsBuilt && !m_IsLeaf)
            {
                for (size_t i = 0; i < 8; i++)
                {
                    if (m_Children[i] != nullptr)
                        if (!m_Children[i]->m_IsEmpty)
                            m_Children[i]->traverse();
                }
            }
        }

        void insert(glm::vec3 pos, T data)
        {
            if (m_IsBuilt)
            {
                int num_col = 0;
                int last_col = -1;
                for (size_t i = 0; i < 8; i++)
                {
                    if (m_Children[i] != nullptr)
                    {
                        if (pPointBoxIntersect(pos, m_Children[i]->m_Position,
                                               m_Children[i]->m_MinMaxPoints))
                        {
                            num_col++;
                            last_col = i;
                        }
                    }
                }

                if (num_col == 1 && last_col != -1)
                {
                    std::cout << "going to " << last_col << " -> ";
                    m_IsEmpty = false;
                    if (m_Children[last_col] != nullptr)
                        m_Children[last_col]->insert(pos, data);
                }
                else if (num_col > 1)
                {
                    insertValue(data);
                    m_IsEmpty = false;
                    std::cout << "found points at depth " << m_Depth << "\n";
                }
                else
                {
                    if (m_IsLeaf)
                    {
                        m_IsEmpty = false;
                        insertValue(data);
                        std::cout << "found points at depth " << m_Depth << "\n";
                    }
                }
            }
        }

    private:
        void pBuild()
        {
            glm::vec3 new_min = (m_MinMaxPoints.min / 2.0f);
            glm::vec3 new_max = (m_MinMaxPoints.max / 2.0f);

            glm::vec3 size = new_max - new_min;
            for (size_t i = 0; i < 8; i++)
            {
                glm::vec3 new_position;
                new_position.x = m_Position.x + (size.x / 2.0) * CUADRANT[i].x;
                new_position.y = m_Position.y + (size.y / 2.0) * CUADRANT[i].y;
                new_position.z = m_Position.z + (size.z / 2.0) * CUADRANT[i].z;

                MIN_MAX_POINTS new_mmp = MIN_MAX_POINTS(new_min, new_max);
                m_Children[i] = new Octree<T>(new_position, new_mmp, m_Depth + 1);
            }

            m_IsBuilt = true;
        }

        bool pPointBoxIntersect(glm::vec3 p, glm::vec3 bp, MIN_MAX_POINTS s)
        {
            glm::vec3 tBoxHalfSize = (s.max - s.min) / 2.0;
            glm::vec3 tBoxMin = bp - tBoxHalfSize;
            glm::vec3 tBoxMax = bp + tBoxHalfSize;
            if (p.x < tBoxMin.x)
            {
                return false;
            }
            if (p.x > tBoxMax.x)
            {
                return false;
            }
            if (p.y < tBoxMin.y)
            {
                return false;
            }
            if (p.y > tBoxMax.y)
            {
                return false;
            }
            if (p.z < tBoxMin.z)
            {
                return false;
            }
            if (p.z > tBoxMax.z)
            {
                return false;
            }

            return true;
        }

        void insertValue(T val) { m_Data.push_back(val); }
    };
}