#pragma once

#include <Entity/Component.h>

#include <Physics/Physics.h>

#include <Physics/PhysicObject.h>
#include <Physics/CubePhysicObject.h>
#include <Physics/SpherePhysicObject.h>
#include <Physics/TriangleMeshPhysicObject.h>
#include <Physics/ClothPhysicObject.h>

#include <iostream>
namespace Epsilon
{
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
            PhysicComponent(float mass, glm::vec3 pos, glm::vec3 scale, Physics::Type type, MIN_MAX_POINTS boundingBox) : mType(PHYSICCOMPONENT), mMass(mass)
            {
                try
                {
                    BaseShape_ptr _shape;
                    DynamicWorld_ptr world = ResourceManager::Get().getPhysicsWorld()->world;
                    switch (type)
                    {
                    case Physics::Type::CUBE:
                        mRigidBodyPointer = std::make_shared<Physics::CubePhysicObject>();
                        world->addRigidBody(std::static_pointer_cast<Physics::CubePhysicObject>(mRigidBodyPointer)->addObject(pos, mass, boundingBox, scale).get());
                        break;
                    case Physics::Type::SPHERE:
                        mRigidBodyPointer = std::make_shared<Physics::SpherePhysicObject>();
                        world->addRigidBody(std::static_pointer_cast<Physics::SpherePhysicObject>(mRigidBodyPointer)->addObject(scale, pos, mass).get());
                        break;
                    case Physics::Type::TRIANGLE_MESH:
                        mRigidBodyPointer = std::make_shared<Physics::TriangleMeshPhysicObject>();
                        break;
                    case Physics::Type::CLOTH:
                        mRigidBodyPointer = std::make_shared<Physics::ClothPhysicObject>();
                        break;
                    default:
                        mRigidBodyPointer = std::make_shared<Physics::SpherePhysicObject>();
                        break;
                    }

                    m_PhysicsWorldPosition = btVector3(pos.x, pos.y, pos.z);

                    //m_PhysicsWorldPosition = mRigidBodyPointer->Body->getCenterOfMassPosition();
                    m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
                    m_PhysicsWorldRotation = mRigidBodyPointer->Body->getOrientation();

                    btTransform transf = getTransform();
                    //btQuaternion rot = btQuaternion(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);

                    glm::quat rot = glm::quat(m_PhysicsWorldRotation.getW(), m_PhysicsWorldRotation.getX(), m_PhysicsWorldRotation.getY(), m_PhysicsWorldRotation.getZ());
                    //btQuaternion rot = btQuaternion(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);
                    //std::cout << " Rotation | x: " << rot.x << " y: " << rot.y << " z: " << rot.z << " w: " << rot.w << std::endl;
                    transf.setRotation(m_PhysicsWorldRotation);
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

            glm::vec3 getPosition()
            {
                return glm::vec3(m_PhysicsWorldPosition.getX(), m_PhysicsWorldPosition.getY(), m_PhysicsWorldPosition.getZ());
            }

            glm::quat getRotation() { return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); }
            glm::vec3 getScale() { return glm::vec3(1.0f); }

            /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
            void Render() {}

            Physics::CollisionInfo CollInfo;
            BaseShape_ptr mRigidBodyPointer = nullptr;
            COMPONENT_TYPE mType;

            btVector3 m_PhysicsWorldPosition;
            btVector3 m_LastPhysicsWorldPosition;
            btQuaternion m_PhysicsWorldRotation;
            btQuaternion m_LastPhysicsWorldRotation;

            glm::quat m_Rotation;
        };

        using PhysicComponent_ptr = std::shared_ptr<PhysicComponent>;
    } // namespace Component
} // namespace Epsilon