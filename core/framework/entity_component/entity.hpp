#pragma once

#include <iterator>
namespace framework {
    class EntityManager;
    class Entity
    {
    public:
        using type = uint32_t;
        using pointer_type = type*;

    public:
        Entity() = default;

        Entity(type id) : mId{ id }
        {}

        type Id() const { return mId; }

        friend EntityManager;

        bool operator<(Entity& other)
        {
            return this->Id() < other.Id();
        }

        bool operator>(Entity& other)
        {
            return this->Id() > other.Id();
        }

    private:
        void Id(type id)
        {
            mId = id;
        }

        type mId;
        type mParent;
    };
}