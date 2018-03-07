#pragma once
#include <Physics/Physics.h>
namespace Physics
{
class PlayerControlledPhysicsObject {
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
}
