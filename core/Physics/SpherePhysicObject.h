#pragma once
#include <Physics/Physics.h>
#include <Physics/PhysicObject.h>

namespace Epsilon
{
	namespace Physics
	{
		class SpherePhysicObject : public PhysicObject
		{
		public:
			SpherePhysicObject() = default;
			virtual ~SpherePhysicObject() {}

		public:
			virtual btRigidBody_ptr addObject(float /* Radius */, float /* Mass */)
			{
				return nullptr;
			}

			virtual btRigidBody_ptr addObject(glm::vec3 /* Radius */, glm::vec3 /* Position */, float /* Mass */);

		private:
			float m_Radius = 1.0f;
			float m_Mass = 0.0f;
			glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

			std::shared_ptr<btSphereShape> SphereShape;
			std::shared_ptr<btMotionState> motionState;
		};
	} // namespace Physics
} // namespace Epsilon