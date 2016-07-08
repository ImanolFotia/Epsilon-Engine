///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
#include <Physics.h>

class ResourceManager;

namespace Game
{
class Player
{
public:
    Player(float x, float y, float z, std::shared_ptr<ResourceManager> resourceManager);
    ~Player() {std::cout << "Deleted Player"<< std::endl;}

public:
    void setPosition(glm::vec3 pos);
    void setDirection(glm::vec3 dir);

    void setHealth(unsigned short health);
    void setSpeed(float speed);

    bool inOnGround();
    std::shared_ptr<btKinematicCharacterController> m_character;
    std::shared_ptr<btPairCachingGhostObject> m_ghostObject;
    void Movement();

private:
    glm::vec3 m_Position;
    glm::vec3 m_Direction;

    unsigned short m_Health;
    float m_Speed;
    bool m_IsOnGround;

    std::shared_ptr<Physics::PlayerControlledPhysicsObject> m_MovementCapsule;
    std::shared_ptr<btRigidBody> m_playerBody;
    std::shared_ptr<btConvexShape> m_playerCapsule;

};
}
