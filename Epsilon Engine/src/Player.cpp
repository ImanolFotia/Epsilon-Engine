///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Player.h>
#include <ResourceManager.h>
#include <sys/KeyBoard.h>
#include <sys/Joystick.h>
#include <iostream>
namespace Game
{
    /** @brief (one liner)
      *
      * (documentation goes here)
      */
    Player::Player(float x, float y, float z, std::shared_ptr<ResourceManager> resourceManager)
    {

        m_rayLambda[0] = 1.0;
        m_rayLambda[1] = 1.0;
        m_turnAngle = 0.0;
        m_maxLinearVelocity = 40.0;
        m_walkVelocity = 10.0; // meters/sec
        m_turnVelocity = 1.0; // radians/sec
        m_PrevDirection = btVector3(0.0, 0.0, 0.0);
        walkSpeed = 0.0f;
        distanceToGround = 0.0;
        btTransform t;  //position and rotation
        t.setIdentity();
        t.setOrigin(btVector3(x, y, z));  //put it to x,y,z coordinates
        m_Position = btVector3(x, y, z);
        m_playerCapsule = (std::shared_ptr<btCapsuleShape>) new btCapsuleShape(1.8, 5.5);
        btVector3 inertia(0,0,0);   //inertia is 0,0,0 for static object, else
        m_playerCapsule->calculateLocalInertia(150.0,inertia);    //it can be determined by this function (for all kind of shapes)
        m_MotionState = (std::shared_ptr<btDefaultMotionState>) new btDefaultMotionState(t);  //set the position (and motion)
        btRigidBody::btRigidBodyConstructionInfo info(150.0,m_MotionState.get(),m_playerCapsule.get(),inertia);  //create the constructioninfo, you can create multiple bodies with the same info
        info.m_restitution = 0.0f;
        info.m_friction = 10.0f;
        m_LocalResourceManagerPointer = resourceManager;
        m_playerBody = (std::shared_ptr<btRigidBody>) new btRigidBody(info);    //let's create the body itself
        m_collinfo = (std::shared_ptr<Physics::CollisionInfo>)new Physics::CollisionInfo();
        m_collinfo->setName(std::string("Player"));
        m_playerBody->setUserPointer(m_collinfo.get());
        m_LocalResourceManagerPointer->m_PhysicsWorld->world->addRigidBody(m_playerBody.get());
        m_playerBody->setCollisionFlags(m_playerBody->getFlags());
        m_playerBody->setActivationState(DISABLE_DEACTIVATION);
        m_playerBody->setSleepingThresholds (0.0, 0.0);
        m_playerBody->setAngularFactor (0.0);
        m_pickedConstraint = 0;

        m_oldPickingPos = btVector3(0,0,0);
        m_hitPos = btVector3(0,0,0);
        m_oldPickingDist = 0.0;


    }

    /** @brief Player is on the ground
      *
      * (documentation goes here)
      */
    bool Player::isOnGround() {
        //return m_rayLambda[0] < btScalar(1.0);
        return distanceToGround < 0.8;
    }

    /** @brief Set the player's speed
      *
      * (documentation goes here)
      */
    void Player::setSpeed(float speed) {}

    /** @brief (one liner)
      *
      * (documentation goes here)
      */
    void Player::setHealth(unsigned short health) {}
    /** @brief (one liner)
      *
      * (documentation goes here)
      */
    void Player::setDirection(glm::vec3 dir) {}
    /** @brief (one liner)
      *
      * (documentation goes here)
      */
    void Player::setPosition(glm::vec3 pos) {}

    /** @brief (one liner)
      *
      * (documentation goes here)
      */
    glm::vec3 Player::getPosition() {
        btVector3 pos = m_playerBody->getCenterOfMassPosition();
        return glm::vec3(pos.getX(), pos.getY() + 2.0, pos.getZ());
    }

    void Player::preStep()
    {
        btTransform xform;
        m_playerBody->getMotionState()->getWorldTransform (xform);
        btVector3 down = -xform.getBasis()[1];
        btVector3 forward = m_Front;//xform.getBasis()[2];
        down.normalize ();
        forward.normalize();

        m_raySource[0] = xform.getOrigin();
        m_raySource[1] = xform.getOrigin();
        float pos = m_playerBody->getCenterOfMassPosition().y();

        m_rayTarget[0] = m_raySource[0] + down *100.0;
        m_rayTarget[1] = m_raySource[1] + forward * 2.5;

        class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
        {
        public:
            ClosestNotMe (btRigidBody* me, btVector3 rayfrom, btVector3 rayTo) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
            {
                m_me = me;
                m_rayFromWorld = rayfrom;
                m_rayToWorld = rayTo;
            }

            virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
            {
                if (rayResult.m_collisionObject == m_me)
                    return 1.0;

                return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
            }
        protected:
            btRigidBody* m_me;
        };

        for(int i = 0; i <2; i++) {
            ClosestNotMe rayCallback(m_playerBody.get(), m_raySource[i], m_rayTarget[i]);
            rayCallback.m_closestHitFraction = 1.0;
            m_LocalResourceManagerPointer->m_PhysicsWorld->world->rayTest(m_raySource[i], m_rayTarget[i], rayCallback);
            if (rayCallback.hasHit())
            {
                btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
                if ((body->isStaticObject()))
                {
                    m_rayLambda[i] = rayCallback.m_closestHitFraction;
                    m_rayPos[i] = rayCallback.m_hitPointWorld;
                }
            } else {
                m_rayLambda[i] = 1.0;
            }
        }

        //std::cout << m_rayPos[1] << std::endl;

        if(isOnGround())
            m_playerBody->setFriction(10.0);
        else
            m_playerBody->setFriction(0.0);

        distanceToGround = glm::length((pos-4.5) - m_rayPos[0].y());

    }



    bool Player::canJump()
    {
        return isOnGround();
    }

    bool Player::HasObstacle()
    {
        return m_rayLambda[1] < btScalar(1.0);
    }

    void Player::Jump() {
        if (!canJump())
            return;

        btVector3 linearVelocity = m_playerBody->getLinearVelocity();
        m_playerBody->setLinearVelocity(btVector3(linearVelocity.getX(), 0.0, linearVelocity.getZ()));
        btTransform xform;
        m_playerBody->getMotionState()->getWorldTransform (xform);
        btVector3 up = xform.getBasis()[1];
        up.normalize ();
        btScalar magnitude = (btScalar(1.0)/m_playerBody->getInvMass()) * btScalar(5.0);
        m_playerBody->applyCentralImpulse (up * magnitude);
    }

    void Player::warp(btVector3 pos)
    {
        btTransform xform;
        this->m_playerBody->getMotionState()->getWorldTransform (xform);
        xform.setOrigin(pos);
        this->m_playerBody->setCenterOfMassTransform(xform);

    }


    void Player::reset()
    {
        btTransform xform;
        this->m_playerBody->getMotionState()->getWorldTransform (xform);
        xform.setOrigin(m_Position);
        this->m_playerBody->setCenterOfMassTransform(xform);

    }

    void Player::Movement(std::shared_ptr<Camera> cam, float dt)
    {
        m_Front = btVector3(cam->getDirection().x, 0.0, cam->getDirection().z);
        btTransform xform;
        this->m_playerBody->getMotionState()->getWorldTransform (xform);

        btVector3 linearVelocity = m_playerBody->getLinearVelocity();
        btScalar speed = m_playerBody->getLinearVelocity().length();
        btVector3 velocity = linearVelocity;
        btVector3 LinearVelocity = m_playerBody->getLinearVelocity();

        btVector3 forwardDir = xform.getBasis()[2];
        forwardDir.normalize ();
        btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);

        btVector3 camDir = btVector3(cam->getDirection().x, 0.0, cam->getDirection().z);
        btVector3 camRight = btVector3(cam->getRight().x, 0.0, cam->getRight().z);
        camDir.normalize();
        camRight.normalize();
        btVector3 pos = m_playerBody->getCenterOfMassPosition();

        if((pos.y() - m_PrevPosition.y()) >=2 )
            higherthannormal = true;
        else
            higherthannormal = false;

        bool moved = false;

        float speedWalk = 12.0, speedSprint = 20.0, currentSpeed = 0.0;

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::LEFT_SHIFT])
        {
            currentSpeed = speedSprint;
        }
        else
        {
            currentSpeed = speedWalk;
        }

        if(!HasObstacle()) {
            if ( (Input::KeyBoard::KEYS[Input::GLFW::Key::W] && isOnGround()) /*|| Input::Joystick::JoystickAxes[1] > 0.0*/)
            {
                moved = true;
                walkSpeed = glm::mix((float)walkSpeed, currentSpeed, 10.0f*dt);
                walkDirection += btVector3(camDir.getX(), 0.0, camDir.getZ());
                velocity += btVector3(camDir.getX()*walkSpeed, -7.0, camDir.getZ()*walkSpeed);
            }
        }

        if ( (Input::KeyBoard::KEYS[Input::GLFW::Key::S] && isOnGround()) /*|| Input::Joystick::JoystickAxes[1] < 0.0*/)
        {
            moved = true;
            walkSpeed = glm::mix((float)walkSpeed, currentSpeed, 10.0f*dt);
            walkDirection += btVector3(-camDir.getX(), 0.0, -camDir.getZ());
            velocity += btVector3(-camDir.getX()*walkSpeed, -7.0, -camDir.getZ()*walkSpeed);
        }

        if ( (Input::KeyBoard::KEYS[Input::GLFW::Key::D] && isOnGround()) /*|| Input::Joystick::JoystickAxes[0] > 0.0*/)
        {
            moved = true;
            walkSpeed = glm::mix((float)walkSpeed, currentSpeed, 10.0f*dt);
            walkDirection += btVector3(camRight.getX(), 0.0, camRight.getZ());
            velocity += btVector3(camRight.getX()*walkSpeed, -7.0, camRight.getZ()*walkSpeed);
        }

        if ( (Input::KeyBoard::KEYS[Input::GLFW::Key::A] && isOnGround()) /*|| Input::Joystick::JoystickAxes[0] < 0.0*/)
        {
            moved = true;
            walkSpeed = glm::mix((float)walkSpeed, currentSpeed, 10.0f*dt);
            walkDirection += btVector3(-camRight.getX(), 0.0, -camRight.getZ());
            velocity += btVector3(-camRight.getX()*walkSpeed, -7.0, -camRight.getZ()*walkSpeed);
        }

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::SPACE] && isOnGround())
        {
            Jump();
        }

        btScalar downVelocity = 0.0;
        btVector3 directionVelocity;

        if(!isOnGround()) {
            if(!Input::KeyBoard::KEYS[Input::GLFW::Key::A] &&
                    !Input::KeyBoard::KEYS[Input::GLFW::Key::W] &&
                    !Input::KeyBoard::KEYS[Input::GLFW::Key::S] &&
                    !Input::KeyBoard::KEYS[Input::GLFW::Key::D]
              )
            {
                walkSpeed = glm::mix((float)walkSpeed, 0.0f,dt);
                directionVelocity = btVector3(m_PrevDirection.x()*walkSpeed * dt, 0.0, m_PrevDirection.z()*walkSpeed * dt);
                //btScalar downVelocity = 0.0;
                //m_playerBody->translate(btVector3(m_PrevDirection.x()*walkSpeed * dt, 0.0, m_PrevDirection.z()*walkSpeed * dt));
                if(distanceToGround < 1.2)
                    downVelocity = 0.10;
            }
            else
            {
                directionVelocity = btVector3(m_PrevDirection.x()*walkSpeed * dt, 0.0, m_PrevDirection.z()*walkSpeed * dt);
                if(distanceToGround < 1.2)
                    downVelocity = 0.10;
            }

        }
        else if(isOnGround()) {
            if(!Input::KeyBoard::KEYS[Input::GLFW::Key::A] &&
                    !Input::KeyBoard::KEYS[Input::GLFW::Key::W] &&
                    !Input::KeyBoard::KEYS[Input::GLFW::Key::S] &&
                    !Input::KeyBoard::KEYS[Input::GLFW::Key::D]
              )
            {
                btScalar downVelocity = 0.0;
                walkSpeed = glm::mix((float)walkSpeed, 0.0f,dt);
                directionVelocity = btVector3(m_PrevDirection.x()*walkSpeed * dt,0.0, m_PrevDirection.z()*walkSpeed * dt);
            }
            else {
                directionVelocity = btVector3(walkDirection.x()*walkSpeed * dt,0.0, walkDirection.z()*walkSpeed * dt);
                /*
                if(distanceToGround < 0.5)
                {
                    downVelocity = distanceToGround;
                }*/
            }
        }

        m_playerBody->translate(btVector3(directionVelocity.x(),-downVelocity, directionVelocity.z()));
        m_playerBody->setLinearVelocity(btVector3(LinearVelocity.x(), -downVelocity + LinearVelocity.y(), LinearVelocity.z()));
        if(isOnGround())
            m_PrevDirection = btVector3(0.0, 0.0, 0.0);


        btScalar deltaPos = glm::length((float)pos.y() - (float)m_rayPos[0].y());
        /*

                    if(distanceToGround < 1.5)
                        m_playerBody->translate(btVector3(walkDirection.x(),-distanceToGround, walkDirection.z())*walkSpeed * dt);
                    else
                        m_playerBody->translate(btVector3(walkDirection.x(),0.0, walkDirection.z())*walkSpeed * dt);
        */

        if (!Input::KeyBoard::KEYS[Input::GLFW::Key::A] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::W] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::S] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::D]) {


        }
        //std::cout << " x: " << m_rayPos[0].x()<< " y: " << m_rayPos[0].y()<< " z: " << m_rayPos[0].z() << std::endl;
        //std::cout << " rayHit: " << m_rayPos[0].y() << std::endl;
        //std::cout << "Distance to ground: " << glm::length((pos.y()-4.5) - m_rayPos[0].y()) << std::endl;
        if(moved)
            m_PrevDirection = walkDirection;

        m_PrevPosition = pos;
    }



    void Player::CheckforPicking(btVector3 from, btVector3 to)
    {
        this->rayTo = to;
        this->rayFrom = btVector3(getPosition().x, getPosition().y, getPosition().z);
        if(Input::KeyBoard::KEYS[Input::GLFW::Key::E])
        {
            if(!active)
            {
                pickObject(from, to);
                active = true;
            }
            moveObject();
        }
        else
        {
            deleteConstraint();
            active = false;
        }
    }


    bool Player::pickObject(btVector3 from, btVector3 to)
    {
        if (m_LocalResourceManagerPointer->m_PhysicsWorld->world==0)
            return false;

        class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
        {
        public:
            ClosestNotMe (btRigidBody* me, btVector3 rayfrom, btVector3 rayTo) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
            {
                m_me = me;
                m_rayFromWorld = rayfrom;
                m_rayToWorld = rayTo;
            }

            virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
            {
                if (rayResult.m_collisionObject == m_me)
                    return 1.0;

                return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
            }
        protected:
            btRigidBody* m_me;
        };
        btVector3 target = rayFrom + rayTo;
        ClosestNotMe rayCallback(m_playerBody.get(), rayFrom, target);

        m_LocalResourceManagerPointer->m_PhysicsWorld->world->rayTest(rayFrom, target, rayCallback);
        if (rayCallback.hasHit())
        {
            //std::cout << "Llega" << std::endl;
            btVector3 pickPos = rayCallback.m_hitPointWorld;
            btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);

            if (body)
            {
                if (!(body->isStaticObject()))
                {
                    m_pickedBody = body;
                    m_savedState = m_pickedBody->getActivationState();
                    m_pickedBody->setActivationState(DISABLE_DEACTIVATION);
                    btVector3 localPivot = body->getCenterOfMassTransform().inverse() * body->getCenterOfMassPosition();
                    btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
                    m_LocalResourceManagerPointer->m_PhysicsWorld->world->addConstraint(p2p, true);
                    m_pickedConstraint = p2p;
                    btScalar mousePickClamping = 1000.f;
                    p2p->m_setting.m_impulseClamp = mousePickClamping;
                    p2p->m_setting.m_tau = 0.1f;
                    //p2p->m_setting.m_damping = 100.0f;
                }
            }

            m_oldPickingPos = target;
            m_hitPos = pickPos;
            m_oldPickingDist = (rayFrom - pickPos).length();
            //std::cout << "m_hitPos: x: " << pickPos.getX() << " y: " << pickPos.getY() << " z: " << pickPos.getZ()  << std::endl;
            //std::cout << "from: x: " << getPosition().getX() << " y: " << from.getY() << " z: " << from.getZ()  << std::endl;
        }
        return false;

    }

    bool Player::moveObject()
    {
        if (m_pickedBody  && m_pickedConstraint)
        {
            btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickedConstraint);
            if (pickCon)
            {
                //keep it at the same picking distance

                btVector3 newPivotB;



                btVector3 dir = rayTo;// - rayFrom;
                dir.normalize();
                dir *= m_oldPickingDist;
                this->pickedbodyangularfactor = m_pickedBody->getAngularVelocity();
                m_pickedBody->setAngularVelocity(btVector3(glm::clamp((float)this->pickedbodyangularfactor.x(), 0.0f, 2.0f),
                                                           glm::clamp((float)this->pickedbodyangularfactor.y(), 0.0f, 2.0f),
                                                           glm::clamp((float)this->pickedbodyangularfactor.z(), 0.0f, 2.0f)));
                newPivotB = rayFrom + dir;
                //std::cout << m_oldPickingDist << std::endl;
                //std::cout << "Pick Pos: x: " << newPivotB.getX() << " y: " << newPivotB.getY() << " z: " << newPivotB.getZ()  << std::endl;
                pickCon->setPivotB(newPivotB);
                return true;
            }
        }
        return false;
    }

    void Player::deleteConstraint()
    {
        if (m_pickedConstraint)
        {
            m_pickedBody->forceActivationState(m_savedState);
            m_pickedBody->activate();
            m_pickedBody->setAngularVelocity(pickedbodyangularfactor);
            m_LocalResourceManagerPointer->m_PhysicsWorld->world->removeConstraint(m_pickedConstraint);
            delete m_pickedConstraint;
            m_pickedConstraint = 0;
            m_pickedBody = 0;
            std::cout << "deleted constraint" << std::endl;
        }
    }

}
