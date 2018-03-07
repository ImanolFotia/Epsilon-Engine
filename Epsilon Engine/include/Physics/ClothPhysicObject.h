#pragma once
#include <Physics/Physics.h>
#include <Physics/PhysicObject.h>

namespace Physics {

	class PhysicObject;

	class ClothPhysicObject : public PhysicObject {
		public:
			ClothPhysicObject() = default;
			virtual ~ClothPhysicObject() {}
		public:
			virtual std::shared_ptr<btRigidBody> addObject();
	};
}
