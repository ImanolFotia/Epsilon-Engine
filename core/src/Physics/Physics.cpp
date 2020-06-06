#include <Physics/Physics.h>
#include <Physics/CollisionInfo.h>
namespace Physics {

	Physics::Physics() {
		collisionConfig = (std::shared_ptr<btCollisionConfiguration>) (new btSoftBodyRigidBodyCollisionConfiguration());
		dispatcher = (std::shared_ptr<btDispatcher>) (new btCollisionDispatcher(collisionConfig.get()));
		broadphase = (std::shared_ptr<btBroadphaseInterface>) (new btDbvtBroadphase());
		solver = (std::shared_ptr<btConstraintSolver>) (new btSequentialImpulseConstraintSolver());
		world = std::make_shared<btSoftRigidDynamicsWorld> (dispatcher.get(),broadphase.get(),solver.get(),collisionConfig.get());

		world->setGravity(btVector3(0,GRAVITY,0));    //gravity on Earth

		softBodyWorldInfo = (std::shared_ptr<btSoftBodyWorldInfo>) new btSoftBodyWorldInfo();
        softBodyWorldInfo->m_broadphase = broadphase.get();
        softBodyWorldInfo->m_dispatcher = dispatcher.get();
        softBodyWorldInfo->m_gravity = world->getGravity();
        softBodyWorldInfo->m_sparsesdf.Initialize();
    }

	void Physics::Update(float deltaTime) {
		//static float acum = 0.0;
		//acum += deltaTime;
		//if(acum >= 1/60){

		mTimeStep = deltaTime;

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

	std::shared_ptr<btSoftRigidDynamicsWorld> Physics::getSoftDynamicsWorld()
	{
	    return std::static_pointer_cast<btSoftRigidDynamicsWorld>(this->world);
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
