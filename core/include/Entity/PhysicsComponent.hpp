#pragma once

#include <Entity/Component.h>

#include <Physics/Physics.h>

#include <Physics/PhysicObject.h>
#include <Physics/CubePhysicObject.h>
#include <Physics/SpherePhysicObject.h>
#include <Physics/TriangleMeshPhysicObject.h>
#include <Physics/ClothPhysicObject.h>

#include <iostream>

namespace Component
{
/// Physic Components
class PhysicComponent : public Component
{
    using BaseShape_ptr = std::shared_ptr<Physics::PhysicObject>;
    using CubeShape_ptr = std::shared_ptr<Physics::CubePhysicObject>;
    using SphereShape_ptr = std::shared_ptr<Physics::SpherePhysicObject>;
    using TriangleMeshShape_ptr = std::shared_ptr<Physics::TriangleMeshPhysicObject>;
    using ClothShape_ptr = std::shared_ptr<Physics::ClothPhysicObject>;
    using DynamicWorld_ptr = std::shared_ptr<btDynamicsWorld>;

public:
    PhysicComponent(float mass, glm::vec3 pos, glm::vec3 scale, Physics::Type type, MIN_MAX_POINTS boundingBox, std::shared_ptr<ResourceManager> rm) : mType(PHYSICCOMPONENT), mMass(mass), mResourceManager(rm)
    {
        try
        {
            BaseShape_ptr _shape;
            DynamicWorld_ptr world = rm->m_PhysicsWorld->world;
            switch (type)
            {
            case Physics::Type::CUBE:
                mRigidBodyPointer = make_shared<Physics::CubePhysicObject>();
                world->addRigidBody(static_pointer_cast<Physics::CubePhysicObject>(mRigidBodyPointer)->addObject(glm::vec3(16, 15, 10), 100.0, boundingBox, scale).get());
                break;
            case Physics::Type::SPHERE:
                mRigidBodyPointer = make_shared<Physics::SpherePhysicObject>();
                world->addRigidBody(static_pointer_cast<Physics::SpherePhysicObject>(mRigidBodyPointer)->addObject(scale, pos, mass).get());
                break;
            case Physics::Type::TRIANGLE_MESH:
                mRigidBodyPointer = make_shared<Physics::TriangleMeshPhysicObject>();
                break;
            case Physics::Type::CLOTH:
                mRigidBodyPointer = make_shared<Physics::ClothPhysicObject>();
                break;
            default:
                mRigidBodyPointer = make_shared<Physics::SpherePhysicObject>();
                break;
            }

            m_PhysicsWorldPosition = btVector3(pos.x, pos.y, pos.z);

            //m_PhysicsWorldPosition = mRigidBodyPointer->Body->getCenterOfMassPosition();
            m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
            m_PhysicsWorldRotation = mRigidBodyPointer->Body->getOrientation();

            btTransform transf = getTransform();
            btQuaternion rot = btQuaternion(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);
            transf.setRotation(rot);
            transf.setOrigin(m_PhysicsWorldPosition);
            setTransform(transf);
            setUserPointer(&CollInfo);
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught in PhysicComponent constructor: " << e.what() << std::endl;
        }
    }
    virtual ~PhysicComponent()
    {
        std::cout << "Physic Destructor" << std::endl;
    }

    COMPONENT_TYPE getType() { return mType; }

    float mMass;

    void Update()
    {

        m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
        m_PhysicsWorldPosition = mRigidBodyPointer->Body->getCenterOfMassPosition();

        /*
                    glm::vec3 pos  = glm::mix(glm::vec3(m_LastPhysicsWorldPosition.getX(), m_LastPhysicsWorldPosition.getY(), m_LastPhysicsWorldPosition.getZ()),
                                                 glm::vec3(m_PhysicsWorldPosition.getX(), m_PhysicsWorldPosition.getY(), m_PhysicsWorldPosition.getZ()),
                                                 rm->timestep*60);

                    m_PhysicsWorldPosition = btVector3(pos.x, pos.y, pos.z);*/
        m_LastPhysicsWorldRotation = m_PhysicsWorldRotation;
        m_PhysicsWorldRotation = mRigidBodyPointer->Body->getOrientation();
    }

    void setUserPointer(void *userPointer)
    {
        mRigidBodyPointer->Body->setUserPointer(userPointer);
    }

    btTransform getTransform()
    {
        return mRigidBodyPointer->Body->getCenterOfMassTransform();
    }

    void setTransform(btTransform t)
    {
        mRigidBodyPointer->Body->setCenterOfMassTransform(t);
    }

    glm::vec3 getPosition() {}
    glm::quat getRotation() {}
    glm::vec3 getScale() {}

    /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
    void Render() {}

    Physics::CollisionInfo CollInfo;
    BaseShape_ptr mRigidBodyPointer = nullptr;
    COMPONENT_TYPE mType;
    std::shared_ptr<ResourceManager> mResourceManager;

    btVector3 m_PhysicsWorldPosition;
    btVector3 m_LastPhysicsWorldPosition;
    btQuaternion m_PhysicsWorldRotation;
    btQuaternion m_LastPhysicsWorldRotation;

    glm::quat m_Rotation;
};
} // namespace Component