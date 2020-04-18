#pragma once

#include <Entity/Component.h>

namespace Component
{
/// NPC/Player Components
class PlayerComponent : public Component
{
public:
    PlayerComponent(bool HasHealth, bool HasGun) : mType(PLAYERCOMPONENT), mHasHealth(HasHealth), mHasGun(HasGun)
    {
    }
    ~PlayerComponent() {}

    COMPONENT_TYPE getType() { return mType; }

    bool hasHealth;
    bool hasGun;
    void Update() {}
    void Render() {}

private:
    COMPONENT_TYPE mType;
    bool mHasHealth;
    bool mHasGun;
};
} // namespace Component