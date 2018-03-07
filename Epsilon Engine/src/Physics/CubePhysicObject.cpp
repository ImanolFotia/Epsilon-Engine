#include <Physics/CubePhysicObject.h>

namespace Physics {
	std::shared_ptr<btRigidBody> CubePhysicObject::addObject(glm::vec3  Position, float  Mass, MIN_MAX_POINTS minmaxpoints, float scale) {
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
		info.m_restitution = 0.1f;
		info.m_friction = 0.8f;
		Body = (std::shared_ptr<btRigidBody>) new btRigidBody(info);

		return Body;
	}
}
