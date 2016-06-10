///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
#include <Physics.h>

namespace Game
{
class Player
{
public:
    Player();
    ~Player() {}

public:
    void setPosition(glm::vec3 pos);
    void setDirection(glm::vec3 dir);

    void setHealth(unsigned short health);
    void setSpeed(float speed);

    bool inOnGround();

private:
    glm::vec3 m_Position;
    glm::vec3 m_Direction;

    unsigned short m_Health;
    float m_Speed;
    bool m_IsOnGround;
    std::shared_ptr<Physics::PlayerControlledPhysicsObject> m_MovementCapsule;

};
}
