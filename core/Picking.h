#pragma once
#include <glm/glm.hpp>
#include <Physics/Physics.h>

class Pick
{
public:
    Pick(std::shared_ptr<btDynamicsWorld>);

    virtual ~Pick(){}

    virtual void CheckforPicking(btVector3, btVector3, btScalar, std::shared_ptr<btDynamicsWorld>);

private:

    virtual bool pickObject(btVector3, btVector3, std::shared_ptr<btDynamicsWorld>);

    virtual bool moveObject();

    virtual void deleteConstraint();

private:

	btRigidBody* m_pickedBody;
	btTypedConstraint* m_pickedConstraint;
    std::shared_ptr<btDynamicsWorld> m_WorldInstance;


    btVector3 m_oldPickingPos;
    btVector3 m_hitPos;
    btVector3 rayTo, rayFrom;
    btScalar m_oldPickingDist;
 
    bool active = false;

    int m_savedState;
};
