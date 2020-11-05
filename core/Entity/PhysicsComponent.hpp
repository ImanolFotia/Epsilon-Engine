#pragma once

#include <pch.hpp>
#include <Entity/Component.h>

#include <Physics/Physics.h>

#include <Physics/CollisionInfo.h>
#include <Physics/PhysicObject.h>
#include <Physics/CubePhysicObject.h>
#include <Physics/SpherePhysicObject.h>
#include <Physics/TriangleMeshPhysicObject.h>
#include <Physics/ClothPhysicObject.h>

#include <Helpers.hpp>

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
            using SoftRigidDynamicsWorld_ptr = std::shared_ptr<btSoftRigidDynamicsWorld>;

            long toHash()
            {
                return std::hash<float>{}(m_PhysicsWorldPosition.x() + m_PhysicsWorldPosition.y() + m_PhysicsWorldPosition.z() + mMass);
            }

        public:
            PhysicComponent(float mass, glm::vec3 pos, glm::vec3 scale, Physics::Type type, MIN_MAX_POINTS boundingBox) : mType(PHYSICCOMPONENT), mMass(mass), mPhysicComponentType(type)
            {
                try
                {
                    BaseShape_ptr _shape;
                    SoftRigidDynamicsWorld_ptr world = ResourceManager::Get().getPhysicsWorld()->getSoftDynamicsWorld();
                    switch (type)
                    {
                    case Physics::Type::CUBE:
                        mRigidBodyPointer = std::make_shared<Physics::CubePhysicObject>();
                        world->addRigidBody(std::static_pointer_cast<Physics::CubePhysicObject>(mRigidBodyPointer)->addObject(pos, mMass, boundingBox, scale).get());
                        break;
                    case Physics::Type::SPHERE:
                        mRigidBodyPointer = std::make_shared<Physics::SpherePhysicObject>();
                        world->addRigidBody(std::static_pointer_cast<Physics::SpherePhysicObject>(mRigidBodyPointer)->addObject(scale, pos, mMass).get());
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

                    m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
                    m_PhysicsWorldRotation = mRigidBodyPointer->Body->getOrientation();

                    btTransform transf = getTransform();

                    glm::quat rot = glm::quat(m_PhysicsWorldRotation.getW(), m_PhysicsWorldRotation.getX(), m_PhysicsWorldRotation.getY(), m_PhysicsWorldRotation.getZ());

                    transf.setRotation(m_PhysicsWorldRotation);
                    transf.setOrigin(m_PhysicsWorldPosition);
                    mCollInfo.setName("Entity_" + std::string(Helpers::to_hex(toHash())));
                    mCollInfo.setType(mPhysicComponentType);
                    setTransform(transf);
                    setUserPointer(&mCollInfo);
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

            bool isStatic() { 
                return mMass > 0 ? false : true;
            }

            void Update()
            {

                m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
                m_PhysicsWorldPosition = mRigidBodyPointer->Body->getCenterOfMassPosition();
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

            const char * getTypeStr()
            {
                switch (mPhysicComponentType)
                {
                case Physics::Type::CUBE:
                    return "Cube";
                    break;
                case Physics::Type::SPHERE:
                    return "Sphere";
                    break;
                case Physics::Type::TRIANGLE_MESH:
                    return "Triangle Mesh";
                    break;
                case Physics::Type::CLOTH:
                    return "Cloth";
                    break;
                }
                return "";
            }

            glm::quat getRotation() { return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); }
            glm::vec3 getScale() { return glm::vec3(1.0f); }

            /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
            void Render() {}

            Physics::CollisionInfo mCollInfo;
            BaseShape_ptr mRigidBodyPointer = nullptr;
            COMPONENT_TYPE mType;

            enum Physics::Type mPhysicComponentType;

            btVector3 m_PhysicsWorldPosition;
            btVector3 m_LastPhysicsWorldPosition;
            btQuaternion m_PhysicsWorldRotation;
            btQuaternion m_LastPhysicsWorldRotation;

            glm::quat m_Rotation;
        };

        using PhysicComponent_ptr = std::shared_ptr<PhysicComponent>;
    } // namespace Component
} // namespace Epsilon