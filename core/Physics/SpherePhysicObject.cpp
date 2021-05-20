#include <Physics/SpherePhysicObject.h>

namespace Epsilon
{
	namespace Physics
	{
		btRigidBody_ptr SpherePhysicObject::addObject(glm::vec3 Radius, glm::vec3 Position, float Mass)
		{
			btTransform t; //position and rotation
			t.setIdentity();
			btVector3 pos = btVector3(Position.x, Position.y, Position.z);
			t.setOrigin(pos); //put it to x,y,z coordinates
			SphereShape = (std::shared_ptr<btSphereShape>)new btSphereShape(Radius.x);
			btVector3 inertia(0, 0, 0); //inertia is 0,0,0 for static object, else
			if (Mass != 0.0)
				SphereShape->calculateLocalInertia(Mass, inertia); //it can be determined by this function (for all kind of shapes)

			btScalar mass = Mass;
			motionState = (std::shared_ptr<btDefaultMotionState>)new btDefaultMotionState(t);					//set the position (and motion)
			btRigidBody::btRigidBodyConstructionInfo info(mass, motionState.get(), SphereShape.get(), inertia); //create the constructioninfo, you can create multiple bodies with the same info
			info.m_restitution = 0.1f;
			info.m_friction = 0.8f;
			
			Body = (btRigidBody_ptr) new btRigidBody(info); //let's create the body itself

			return Body;
		}
	} // namespace Physics
} // namespace Epsilon
