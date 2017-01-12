#include <Picking.h>
#include <sys/KeyBoard.h>

Pick::Pick(std::shared_ptr<btDynamicsWorld> world)
{
    m_WorldInstance = world;

    m_oldPickingPos = btVector3(0,0,0);
    m_hitPos = btVector3(0,0,0);
    m_oldPickingDist = 0.0;
}

void Pick::CheckforPicking(btVector3 from, btVector3 to, btScalar length, std::shared_ptr<btDynamicsWorld> world)
{
    this->rayTo = to;
    this->rayFrom = from;
    if(Input::KeyBoard::KEYS[Input::GLFW::Key::E])
    {
        if(!active)
        {
            pickObject(from, to, world);
            active = true;
        }
        moveObject();
    }
    else
    {
        deleteConstraint();
        active = false;
    }
}


bool Pick::pickObject(btVector3 from, btVector3 to, std::shared_ptr<btDynamicsWorld> world)
{
    if (m_WorldInstance==0)
        return false;


    btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

    m_WorldInstance->rayTest(from, to, rayCallback);
    if (rayCallback.hasHit())
    {
        btVector3 pickPos = rayCallback.m_hitPointWorld;
        btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
        //std::cout << "Llega" << std::endl;
        if (body)
        {
            if (!(body->isStaticObject() || body->isKinematicObject()))
            {

                m_pickedBody = body;
                m_savedState = m_pickedBody->getActivationState();
                m_pickedBody->setActivationState(DISABLE_DEACTIVATION);
                btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
                std::cout << "Pick Pos: x: " << localPivot.getX() << " y: " << localPivot.getY() << " z: " << localPivot.getZ()  << std::endl;
                btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
                m_WorldInstance->addConstraint(p2p, true);
                m_pickedConstraint = p2p;
                btScalar mousePickClamping = 30.f;
                p2p->m_setting.m_impulseClamp = mousePickClamping;
                p2p->m_setting.m_tau = 0.001f;
                //p2p->m_setting.m_damping = 100.0f;
            }
        }


        m_oldPickingPos = to;
        m_hitPos = pickPos;
        m_oldPickingDist = (from - pickPos).length();
        std::cout << "m_hitPos: x: " << pickPos.getX() << " y: " << pickPos.getY() << " z: " << pickPos.getZ()  << std::endl;
        std::cout << "from: x: " << from.getX() << " y: " << from.getY() << " z: " << from.getZ()  << std::endl;
    }
    return false;

}

bool Pick::moveObject()
{
    if (m_pickedBody  && m_pickedConstraint)
    {
        btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickedConstraint);
        if (pickCon)
        {
            //keep it at the same picking distance

            btVector3 newPivotB;



            btVector3 dir = rayTo;// - rayFrom;
            dir.normalize();
            dir *= m_oldPickingDist;

            newPivotB = rayFrom + dir;
            //std::cout << m_oldPickingDist << std::endl;
            //std::cout << "Pick Pos: x: " << newPivotB.getX() << " y: " << newPivotB.getY() << " z: " << newPivotB.getZ()  << std::endl;
            pickCon->setPivotB(newPivotB);
            return true;
        }
    }
    return false;
}

void Pick::deleteConstraint()
{
    if (m_pickedConstraint)
    {
        m_pickedBody->forceActivationState(m_savedState);
        m_pickedBody->activate();
        m_WorldInstance->removeConstraint(m_pickedConstraint);
        delete m_pickedConstraint;
        m_pickedConstraint = 0;
        m_pickedBody = 0;
        std::cout << "deleted constraint" << std::endl;
    }
}
