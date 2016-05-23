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

bool Physics::addCube(float x, float y, float z)
{
    try
    {
        btTransform t;
        t.setIdentity();
        btVector3 SpawnPosition = btVector3(x,y,z);
        t.setOrigin(SpawnPosition);
        btBoxShape* Cube = new btBoxShape(btVector3(1,1,1));

        btVector3 inertia(0,0,0);
        btScalar mass = 20.0;

        btMotionState* motion = new btDefaultMotionState(t);

        btRigidBody::btRigidBodyConstructionInfo info(mass,motion,Cube,inertia);

        btRigidBody* body = new btRigidBody(info);

        body->setLinearVelocity((btVector3(0,0,0)));

        this->world->addRigidBody(body);
    }
    catch(...)
    {
        throw;
    }

}

bool Physics::addTriangleMesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices)
{

    btCollisionShape* shape = nullptr;
    btTriangleMesh* trimesh = new btTriangleMesh();
    btTransform a;
    a.setIdentity();
    a.setOrigin(btVector3(0,0,0));
    btVector3 localScaling(1.f,1.f,1.f);

    for ( int i = 0; i < indices.size() / 3 ; i++)
    {
        int index0 = indices[i*3];
        int index1 = indices[i*3+1];
        int index2 = indices[i*3+2];

        glm::vec3 VERTEX0 = vertices.at(index0);
        glm::vec3 VERTEX1 = vertices.at(index1);
        glm::vec3 VERTEX2 = vertices.at(index2);

        trimesh->addTriangle(btVector3(VERTEX0.x, VERTEX0.y, VERTEX0.z),btVector3(VERTEX1.x, VERTEX1.y, VERTEX1.z),btVector3(VERTEX2.x, VERTEX2.y, VERTEX2.z));
    }

    shape = new btBvhTriangleMeshShape(trimesh,true);

    btMotionState* motion0=new btDefaultMotionState(a);
    btRigidBody::btRigidBodyConstructionInfo info0(0.0,motion0,shape, btVector3(0,0,0));
    btRigidBody* p0 = new btRigidBody(info0);
    world->addRigidBody(p0);
}

void Physics::Update(float deltaTime)
{
    world->stepSimulation(deltaTime);
}

std::shared_ptr<btRigidBody> CubePhysicObject::addObject(glm::vec3  Position , float  Mass , float  Size )
{
    btTransform t;  //position and rotation
    t.setIdentity();
    btVector3 pos = btVector3(Position.x, Position.y, Position.z);
    t.setOrigin(pos);  //put it to x,y,z coordinates
    CubeShape = (std::shared_ptr<btBoxShape>) new btBoxShape(btVector3(1,1,1));    //it's a sphere, so use sphereshape

    btVector3 inertia(0,0,0);   //inertia is 0,0,0 for static object, else
    if(Mass!=0.0)
        CubeShape->calculateLocalInertia(Mass,inertia);    //it can be determined by this function (for all kind of shapes)

    btScalar mass = Mass;
    motionState = (std::shared_ptr<btDefaultMotionState>) new btDefaultMotionState(t);  //set the position (and motion)
    btRigidBody::btRigidBodyConstructionInfo info(mass,motionState.get(),CubeShape.get(),inertia);  //create the constructioninfo, you can create multiple bodies with the same info
    Body = (std::shared_ptr<btRigidBody>) new btRigidBody(info);    //let's create the body itself

    return Body;
}
}
