#include <Physics/Physics.h>
#include <Physics/CollisionInfo.h>
namespace Physics {

	Physics::Physics() {
		collisionConfig = (std::shared_ptr<btCollisionConfiguration>) (new btDefaultCollisionConfiguration());
		dispatcher = (std::shared_ptr<btDispatcher>) (new btCollisionDispatcher(collisionConfig.get()));
		broadphase = (std::shared_ptr<btBroadphaseInterface>) (new btDbvtBroadphase());
		solver = (std::shared_ptr<btConstraintSolver>) (new btSequentialImpulseConstraintSolver());
		world = (std::shared_ptr<btDynamicsWorld>) (new btDiscreteDynamicsWorld(dispatcher.get(),broadphase.get(),solver.get(),collisionConfig.get()));

		world->setGravity(btVector3(0,GRAVITY,0));    //gravity on Earth
	}

	void Physics::Update(float deltaTime) {
		//static float acum = 0.0;
		//acum += deltaTime;
		//if(acum >= 1/60){

		world->stepSimulation(deltaTime, 5, 1.0/300.0);
		//acum = 0.0;
		//}
	}


	std::string Physics::getCollisionObjectName(btVector3 rayPosition, btVector3 rayTarget) {
		btVector3 target =  btVector3(rayTarget.getX() * 1000, rayTarget.getY() * 1000, rayTarget.getZ() * 1000);
		btCollisionWorld::ClosestRayResultCallback RayCallback(rayPosition, target);

		world->rayTest(rayPosition, target, RayCallback);
		if(RayCallback.hasHit()) {
			try {
				void *var;
				var =  RayCallback.m_collisionObject->getUserPointer();
				if(var == nullptr)
					return "UNDEFINED_COLLISION_INFO";

				CollisionInfo* CollInfo;
				CollInfo = reinterpret_cast<CollisionInfo*>(var);
				return CollInfo->getName();
			} catch(...) {
				throw;
			}
		} else
			return "Skybox";
	}

	glm::vec3 Physics::getCollisionPosition(btVector3 rayPosition, btVector3 rayTarget) {
		btVector3 target =  btVector3(rayTarget.getX() * 1000, rayTarget.getY() * 1000, rayTarget.getZ() * 1000);
		btCollisionWorld::ClosestRayResultCallback RayCallback(rayPosition, target);

		world->rayTest(rayPosition, target, RayCallback);
		if(RayCallback.hasHit()) {
			btVector3 coll = RayCallback.m_hitPointWorld;
			return glm::vec3(coll.getX(), coll.getY(),coll.getZ());
		} else
			return glm::vec3(0,0,0);
	}

}
