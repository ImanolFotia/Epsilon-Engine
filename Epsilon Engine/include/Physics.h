#pragma once

#include <glm/glm.hpp>
#include <EpsilonMemory.h>
#include <Types.h>
#include <Model.h>

#include <vector>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.h>
#include <BulletDynamics/Featherstone/btMultiBodyConstraintSolver.h>
#include <LinearMath/btTransform.h>

namespace Physics
{
    class CollisionInfo;

    class Physics
    {
    public:
        Physics();

        ~Physics()
        {
        //std::cout << "Deleted Physics" << std::endl;

        }

        void Update(float);

        std::string getCollisionObjectName(btVector3 rayPosition, btVector3 rayTarget);
        glm::vec3 getCollisionPosition(btVector3 rayPosition, btVector3 rayTarget);

        std::shared_ptr<btDynamicsWorld> world;
        std::shared_ptr<btBroadphaseInterface> broadphase;

    private:

        std::shared_ptr<btDispatcher> dispatcher;
        std::shared_ptr<btCollisionConfiguration> collisionConfig;
        std::shared_ptr<btConstraintSolver> solver;
    protected:

    };

    class PhysicObject
    {
    public:
        PhysicObject() = default;
        virtual ~PhysicObject() {}

    public:
        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */) = 0;
        virtual std::shared_ptr<btRigidBody> addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/) = 0;
        virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */) = 0;
        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, MIN_MAX_POINTS /* Bounding Box */, float /* Scale*/) = 0;
        virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, float) = 0;
        std::shared_ptr<btRigidBody> Body;
    };

    class CubePhysicObject : public PhysicObject
    {
    public:
        CubePhysicObject() = default;
        virtual ~CubePhysicObject() {}

    public:
        virtual std::shared_ptr<btRigidBody> addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/) {}

        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, MIN_MAX_POINTS /* Bounding Box */, float /* Scale*/);

    private:

        float m_Size = 1.0f;
        float m_Mass = 0.0f;
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

        std::shared_ptr<btBoxShape> CubeShape;
        std::shared_ptr<btMotionState> motionState;

        virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */) {}
        virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, float) {}
        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */) {}
    };

    class SpherePhysicObject : public PhysicObject
    {
    public:
        SpherePhysicObject() = default;
        virtual ~SpherePhysicObject() {}

    public:
        virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */) {}

        virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */,  glm::vec3 /* Position */, float /* Mass */);

    private:

        float m_Radius = 1.0f;
        float m_Mass = 0.0f;
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

        std::shared_ptr<btSphereShape> SphereShape;
        std::shared_ptr<btMotionState> motionState;

        virtual std::shared_ptr<btRigidBody> addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/) {}
        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, MIN_MAX_POINTS /* Bounding Box */, float /* Scale*/) {}
        virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, float) {}
        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */) {}
    };

    class TriangleMeshPhysicObject : public PhysicObject
    {
    public:
        TriangleMeshPhysicObject() = default;
        virtual ~TriangleMeshPhysicObject() {}

    public:
        virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, glm::vec3 /* Position */,float /* Mass */) {}

        virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, float Scaling);

    private:

        float m_Mass = 0.0f;
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

        std::shared_ptr<btMotionState> motionState;
        std::shared_ptr<btCollisionShape> TriangleMeshShape;
        std::shared_ptr<btTriangleMesh> trimesh;

    private:
        virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */) {}
        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */) {}
        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, MIN_MAX_POINTS /* Bounding Box */, float /* Scale*/) {}
        virtual std::shared_ptr<btRigidBody> addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/) {}
    };

    class PlayerControlledPhysicsObject
    {
    public:
        PlayerControlledPhysicsObject(btScalar x, btScalar y, btScalar z);
        ~PlayerControlledPhysicsObject() {}

        glm::vec3 getPosition();
        glm::vec3 getOrientation();

        bool isOnGround();
        float getSpeed();

    private:
        btScalar sizeX, sizeY, sizeZ;
        std::shared_ptr<btMotionState> m_motionState;
        std::shared_ptr<btCollisionShape> capsuleShape;

        std::shared_ptr<btRigidBody> m_playerbody;
        std::shared_ptr<btPairCachingGhostObject> m_ghostObject;
        std::shared_ptr<btCharacterControllerInterface> m_character;
    };

    class CollisionInfo
    {
    public:
        CollisionInfo()
        {
            m_Name = "";
        }
        ~CollisionInfo() {}

        std::string getName()
        {
            return m_Name;
        }

        void setName(std::string name)
        {
            m_Name = name;
        }

    private:
        std::string m_Name;
    };

}






