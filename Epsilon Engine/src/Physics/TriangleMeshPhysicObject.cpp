#include <Physics/TriangleMeshPhysicObject.h>

namespace Physics {
	std::shared_ptr<btRigidBody> TriangleMeshPhysicObject::addObject(std::vector<glm::vec3> inVertices, std::vector<unsigned int> inIndices, float Scaling) {
		trimesh = (std::shared_ptr<btTriangleMesh>)new btTriangleMesh();
		btTransform a;
		a.setIdentity();
		a.setOrigin(btVector3(0,0,0));
		btVector3 localScaling(Scaling,Scaling,Scaling);
		for (unsigned int i = 0; i < inIndices.size() / 3 ; i++) {
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
		info.m_restitution = 1.0f;
		info.m_friction = 0.8f;
		Body = (std::shared_ptr<btRigidBody>) new btRigidBody(info);

		return Body;
	}
}
