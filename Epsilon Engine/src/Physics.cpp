#include <Physics.h>

namespace Physics
{

Physics::Physics()
{
    collisionConfig = (std::shared_ptr<btCollisionConfiguration>) (new btDefaultCollisionConfiguration());
    dispatcher = (std::shared_ptr<btDispatcher>) (new btCollisionDispatcher(collisionConfig.get()));
    broadphase = (std::shared_ptr<btBroadphaseInterface>) (new btDbvtBroadphase());
    solver = (std::shared_ptr<btConstraintSolver>) (new btSequentialImpulseConstraintSolver());
    world = (std::shared_ptr<btDynamicsWorld>) (new btDiscreteDynamicsWorld(dispatcher.get(),broadphase.get(),solver.get(),collisionConfig.get()));

    world->setGravity(btVector3(0,GRAVITY,0));    //gravity on Earth
}

void Physics::Update(float deltaTime)
{
    //static float acum = 0.0;
    //acum += deltaTime;
    //if(acum >= 1/60){

        world->stepSimulation(deltaTime, 5, 1.0/300.0);
        //acum = 0.0;
    //}
}


std::string Physics::getCollisionObjectName(btVector3 rayPosition, btVector3 rayTarget)
{
    btVector3 target =  btVector3(rayTarget.getX() * 1000, rayTarget.getY() * 1000, rayTarget.getZ() * 1000);
    btCollisionWorld::ClosestRayResultCallback RayCallback(rayPosition, target);

    world->rayTest(rayPosition, target, RayCallback);
    if(RayCallback.hasHit())
    {
        try{
        void *var;
        var =  RayCallback.m_collisionObject->getUserPointer();
        if(var == nullptr)
            return "UNDEFINED_COLLISION_INFO";

        CollisionInfo* CollInfo;
        CollInfo = reinterpret_cast<CollisionInfo*>(var);
        return CollInfo->getName();
        }
        catch(...)
        {
            throw;
        }
    }
    else
        return "Skybox";
}

glm::vec3 Physics::getCollisionPosition(btVector3 rayPosition, btVector3 rayTarget)
{
    btVector3 target =  btVector3(rayTarget.getX() * 1000, rayTarget.getY() * 1000, rayTarget.getZ() * 1000);
    btCollisionWorld::ClosestRayResultCallback RayCallback(rayPosition, target);

    world->rayTest(rayPosition, target, RayCallback);
    if(RayCallback.hasHit())
    {
        btVector3 coll = RayCallback.m_hitPointWorld;
        return glm::vec3(coll.getX(), coll.getY(),coll.getZ());
    }
    else
        return glm::vec3(0,0,0);
}

std::shared_ptr<btRigidBody> CubePhysicObject::addObject(glm::vec3  Position , float  Mass , MIN_MAX_POINTS minmaxpoints, float scale)
{
    btTransform t;
    t.setIdentity();
    btVector3 pos = btVector3(Position.x, Position.y, Position.z);
    t.setOrigin(pos);
    CubeShape = (std::shared_ptr<btBoxShape>) new btBoxShape(btVector3(scale * (glm::length(minmaxpoints.MAX_X - minmaxpoints.MIN_X)/2), scale * (glm::length(minmaxpoints.MAX_Y - minmaxpoints.MIN_Y)/2), scale * (glm::length(minmaxpoints.MAX_Z - minmaxpoints.MIN_Z)/2)));
    btVector3 inertia(0,0,0);
    if(Mass!=0.0)
        CubeShape->calculateLocalInertia(Mass,inertia);

    btScalar mass = Mass;
    motionState = (std::shared_ptr<btDefaultMotionState>) new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass,motionState.get(),CubeShape.get(),inertia);
    info.m_restitution = 1.0f;
    info.m_friction = 0.8f;
    Body = (std::shared_ptr<btRigidBody>) new btRigidBody(info);

    return Body;
}

std::shared_ptr<btRigidBody> SpherePhysicObject::addObject(float  Radius ,  glm::vec3 Position , float  Mass )
{
    btTransform t;  //position and rotation
    t.setIdentity();
    btVector3 pos = btVector3(Position.x, Position.y, Position.z);
    t.setOrigin(pos);  //put it to x,y,z coordinates
    SphereShape = (std::shared_ptr<btSphereShape>) new btSphereShape(Radius);
    btVector3 inertia(0,0,0);   //inertia is 0,0,0 for static object, else
    if(Mass!=0.0)
        SphereShape->calculateLocalInertia(Mass,inertia);    //it can be determined by this function (for all kind of shapes)

    btScalar mass = Mass;
    motionState = (std::shared_ptr<btDefaultMotionState>) new btDefaultMotionState(t);  //set the position (and motion)
    btRigidBody::btRigidBodyConstructionInfo info(mass,motionState.get(),SphereShape.get(),inertia);  //create the constructioninfo, you can create multiple bodies with the same info
    info.m_restitution = 2.0f;
    info.m_friction = 0.8f;
    Body = (std::shared_ptr<btRigidBody>) new btRigidBody(info);    //let's create the body itself

    return Body;
}

std::shared_ptr<btRigidBody> TriangleMeshPhysicObject::addObject(std::vector<glm::vec3> inVertices, std::vector<unsigned int> inIndices, float Scaling)
{
    trimesh = (std::shared_ptr<btTriangleMesh>)new btTriangleMesh();
    btTransform a;
    a.setIdentity();
    a.setOrigin(btVector3(0,0,0));
    btVector3 localScaling(Scaling,Scaling,Scaling);
    for ( int i = 0; i < inIndices.size() / 3 ; i++)
    {
        int index0 = inIndices[i*3];
        int index1 = inIndices[i*3+1];
        int index2 = inIndices[i*3+2];

        glm::vec3 VERTEX0 = inVertices.at(index0);
        glm::vec3 VERTEX1 = inVertices.at(index1);
        glm::vec3 VERTEX2 = inVertices.at(index2);

        trimesh->addTriangle(btVector3(VERTEX0.x, VERTEX0.y, VERTEX0.z) * localScaling,btVector3(VERTEX1.x, VERTEX1.y, VERTEX1.z) * localScaling,btVector3(VERTEX2.x, VERTEX2.y, VERTEX2.z) * localScaling);
    }

    TriangleMeshShape = (std::shared_ptr<btBvhTriangleMeshShape>)new btBvhTriangleMeshShape(trimesh.get(),true);

    motionState = (std::shared_ptr<btDefaultMotionState>) new btDefaultMotionState(a);
    btRigidBody::btRigidBodyConstructionInfo info(0.0,motionState.get(),TriangleMeshShape.get(), btVector3(0,0,0));
    info.m_restitution = 0.2f;
    info.m_friction = 0.8f;
    Body = (std::shared_ptr<btRigidBody>) new btRigidBody(info);

    return Body;
}
}
