#pragma once
#include <Physics/Physics.h>


namespace Physics {
	class PhysicObject {
		public:
			PhysicObject() = default;
			virtual ~PhysicObject() {}

		public:
			virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */) {}
			virtual std::shared_ptr<btRigidBody> addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/) {}
			virtual std::shared_ptr<btRigidBody> addObject(float /* Radius */, float /* Mass */) {}
			virtual std::shared_ptr<btRigidBody> addObject(glm::vec3 /* Position */, float /* Mass */, MIN_MAX_POINTS /* Bounding Box */, float /* Scale*/) {}
			virtual std::shared_ptr<btRigidBody> addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, float) {}
			std::shared_ptr<btRigidBody> Body;
	};
}

#include <Physics/ClothPhysicObject.h>
#include <Physics/CubePhysicObject.h>
#include <Physics/SpherePhysicObject.h>
#include <Physics/TriangleMeshPhysicObject.h>
