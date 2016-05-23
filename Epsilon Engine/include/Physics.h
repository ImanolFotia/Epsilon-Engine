#pragma once

#include <glm/glm.hpp>
#include <EpsilonMemory.h>
#include <Types.h>

#include <vector>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
namespace Physics
{
class Physics
{
public:
    Physics();

    ~Physics()
    {

    }

    void Update(float);

    bool addCube(float, float, float);

    bool addSphere(float);

    bool addCone(float, float, float);

    bool addCylinder(float, float, float);

    bool addTriangleMesh(std::vector<glm::vec3>, std::vector<unsigned int>);

    std::shared_ptr<btDynamicsWorld> world;

private:

    std::shared_ptr<btDispatcher> dispatcher;
    std::shared_ptr<btCollisionConfiguration> collisionConfig;
    std::shared_ptr<btBroadphaseInterface> broadphase;
    std::shared_ptr<btConstraintSolver> solver;

protected:

};

class PhysicObject
{
public:
    PhysicObject(){}
    virtual ~PhysicObject(){};

public:
    virtual btRigidBody* addObject() = 0;
    virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */) = 0;
    virtual std::shared_ptr<btRigidBody> addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/) = 0;
    virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */) = 0;
    virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */) = 0;
    std::shared_ptr<btRigidBody> Body;
};

class CubePhysicObject : public PhysicObject
{
    public:
    CubePhysicObject(){}
    ~CubePhysicObject() {}

    public:
        virtual std::shared_ptr<btRigidBody> addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/){}

        virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */);

    private:

        float m_Size = 1.0f;
        float m_Mass = 0.0f;
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

        std::shared_ptr<btBoxShape> CubeShape;
        std::shared_ptr<btMotionState> motionState;


    virtual btRigidBody* addObject(){}
    virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */){}
    virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */){}
};

class SpherePhysicObject : public PhysicObject
{
    public:
    SpherePhysicObject();
    virtual ~SpherePhysicObject() {}

    public:
        virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */);

        virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */,  glm::vec3 /* Position */, float /* Mass */);

    private:

        float m_Radius = 1.0f;
        float m_Mass = 0.0f;
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

        std::shared_ptr<btSphereShape> CubeShape;
        std::shared_ptr<btMotionState> motionState;
};

class TriangleMeshPhysicObject : public PhysicObject
{
    public:
    TriangleMeshPhysicObject();
    virtual ~TriangleMeshPhysicObject() {}

    public:
        virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, glm::vec3 /* Position */,float /* Mass */);

        virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */);

    private:

        float m_Mass = 0.0f;
        glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

        std::shared_ptr<btMotionState> motionState;
        std::shared_ptr<btCollisionShape> shape;
        std::shared_ptr<btTriangleMesh> trimesh;
};

}






