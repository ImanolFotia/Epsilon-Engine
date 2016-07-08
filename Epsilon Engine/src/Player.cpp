///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Player.h>
#include <ResourceManager.h>
#include <KeyBoard.h>
namespace Game
{
/** @brief (one liner)
  *
  * (documentation goes here)
  */
Player::Player(float x, float y, float z, std::shared_ptr<ResourceManager> resourceManager)
{
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(x, y, z));

    m_ghostObject = (std::shared_ptr<btPairCachingGhostObject>) new btPairCachingGhostObject();
    m_ghostObject->setWorldTransform(t);
    resourceManager->m_PhysicsWorld->broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    m_playerCapsule = (std::shared_ptr<btCapsuleShape>) new btCapsuleShape(3, 5);
    m_ghostObject->setCollisionShape(m_playerCapsule.get());
    m_ghostObject->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
    btScalar stepHeight = btScalar(3.0f);
    m_character = (std::shared_ptr<btKinematicCharacterController>)new btKinematicCharacterController(m_ghostObject.get(),m_playerCapsule.get(),stepHeight);
    resourceManager->m_PhysicsWorld->world->addCollisionObject(m_ghostObject.get(),btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter|btBroadphaseProxy::KinematicFilter);

    resourceManager->m_PhysicsWorld->world->addAction(m_character.get());

}

/** @brief Player is on the ground
  *
  * (documentation goes here)
  */
bool Player::inOnGround(){}

/** @brief Set the player's speed
  *
  * (documentation goes here)
  */
void Player::setSpeed(float speed){}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void Player::setHealth(unsigned short health){}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
void Player::setDirection(glm::vec3 dir){}
/** @brief (one liner)
  *
  * (documentation goes here)
  */
void Player::setPosition(glm::vec3 pos){}


void Player::Movement()
{
    m_character->setWalkDirection(btVector3(0,0,0));

    //m_character->setVelocityForTimeInterval(btVector3(10,10,10), 0.016f);
    m_character->setMaxJumpHeight(300);
    m_character->setFallSpeed(1000);
    m_character->setJumpSpeed(30);
    m_character->setGravity(100);
    btVector3 dir;

	if ( Input::KeyBoard::KEYS[Input::GLFW::Key::W])
    {
         dir = btVector3(0.9, 0, 0.0);
    }

    if ( Input::KeyBoard::KEYS[Input::GLFW::Key::S] )
    {
         dir = btVector3(-0.9, 0, 0.0);
    }

    if ( Input::KeyBoard::KEYS[Input::GLFW::Key::D] )
    {
         dir = btVector3(0.0, 0, 0.9);
    }

    if ( Input::KeyBoard::KEYS[Input::GLFW::Key::A] )
    {
         dir = btVector3(0.0, 0, -0.9);
    }
    if(Input::KeyBoard::KEYS[Input::GLFW::Key::SPACE] == GLFW_PRESS && m_character->onGround() && m_character->canJump())
    {
        m_character->jump();
    }

    m_character->setWalkDirection((dir));
}
}
