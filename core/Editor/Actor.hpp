#pragma once

#include <Transform.hpp>
#include <types.h>

namespace Epsilon::Editor
{

    enum ACTOR_TYPE
    {
        LIGHT,
        ENTITY,
        CAMERA
    };

    class Actor
    {
        Transform mTransform;
        std::string mName;
        ACTOR_TYPE mType;
        bool mActiveComponents[Component::COMPONENT_TYPE::SIZE] = {};

    public:
        Actor(Transform t, std::string n, ACTOR_TYPE ty)
            : mTransform(t), mName(n), mType(ty)
        {
        }

        auto Transform() { return mTransform; }
        auto Name() { return mName; }
        auto Type() { return mType; }
        auto ActiveComponents() { return mActiveComponents; }
        auto SetActiveComponents(Component::COMPONENT_TYPE c, bool s) { mActiveComponents[c] = s; }
    };
}