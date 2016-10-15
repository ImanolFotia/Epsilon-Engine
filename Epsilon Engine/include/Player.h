///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
#include <Physics.h>
#include <camera.h>
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

    glm::vec3 getPosition();

    bool isOnGround();
    void reset();
    bool canJump();
    void Jump();
    void warp(btVector3);
    bool HasObstacle();

    void preStep();
    void Movement(std::shared_ptr<Camera> cam, float dt);

private:
    btVector3 m_Position;
    btVector3 m_Direction;

    btVector3 m_PrevDirection;
    btVector3 m_Front;
	btVector3 m_raySource[2];
	btVector3 m_rayTarget[2];
	btScalar m_rayLambda[2];
	btVector3 m_rayNormal[2];
	btVector3 m_rayPos[2];
    float distanceToGround;
    btScalar walkSpeed;
	btScalar m_turnAngle;

	btScalar m_maxLinearVelocity;
	btScalar m_walkVelocity;
	btScalar m_turnVelocity;
    btVector3 m_rayHit;
    unsigned short m_Health;
    float m_Speed;
    bool m_IsOnGround;

    std::shared_ptr<btRigidBody> m_playerBody;
    std::shared_ptr<btCapsuleShape> m_playerCapsule;
    std::shared_ptr<btMotionState> m_MotionState;
    std::shared_ptr<ResourceManager> m_LocalResourceManagerPointer;

};
}
