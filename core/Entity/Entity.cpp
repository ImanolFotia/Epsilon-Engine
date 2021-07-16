#include <Entity/Entity.h>

//make available only the types we are using within the namespace, not the whole namespace
//to reduce typing, and make the code more readable

using Epsilon::Component::PhysicComponent;
using Epsilon::Component::RenderComponent;
using Epsilon::Component::SoundComponent;
using Epsilon::Component::TransformComponent;

using Epsilon::Component::CLOTHCOMPONENT;
using Epsilon::Component::PHYSICCOMPONENT;
using Epsilon::Component::RENDERCOMPONENT;
using Epsilon::Component::SOUNDCOMPONENT;
using Epsilon::Component::TRANSFORMCOMPONENT;

namespace Epsilon
{
    EntityBase::EntityBase(glm::vec3 pos, glm::vec3 sc, glm::quat rot)
    {
        ComponentList[TRANSFORMCOMPONENT] = std::make_shared<TransformComponent>(pos, sc, rot);
        Transform = std::static_pointer_cast<Component::TransformComponent>(ComponentList[TRANSFORMCOMPONENT]);
    }

    void EntityBase::Update()
    {
        if (mFunction)
            mFunction(this);

        if (mHasSoundComponent)
        {
            glm::vec3 position = this->getPosition();
            std::static_pointer_cast<SoundComponent>(ComponentList.at(SOUNDCOMPONENT))->setPosition(position);
        }

        auto tcomp = std::static_pointer_cast<Component::TransformComponent>(ComponentList[Component::TRANSFORMCOMPONENT]);
        for (auto &c : ComponentList)
        {
            c.second->Update();

            if (c.second->getType() == Component::PHYSICCOMPONENT &&
                (tcomp->externally_modified_position || tcomp->externally_modified_scale || tcomp->externally_modified_rotation))
            {
                std::cout << "Updating after modificed" << std::endl;
                auto pcomp = std::static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT]);

                btTransform t = pcomp->getTransform();

                if (tcomp->externally_modified_position)
                {
                    auto pos = tcomp->Position();

                    pcomp->m_LastPhysicsWorldPosition = pcomp->m_PhysicsWorldPosition;
                    pcomp->m_PhysicsWorldPosition = btVector3(pos.x, pos.y, pos.z);

                    t.setOrigin(pcomp->m_PhysicsWorldPosition);
                }
                if (tcomp->externally_modified_rotation)
                {
                    auto rot = tcomp->Rotation();
                    pcomp->m_LastPhysicsWorldRotation = pcomp->m_PhysicsWorldRotation;
                    pcomp->m_PhysicsWorldRotation = btQuaternion(rot.w, rot.x, rot.y, rot.z);

                    t.setRotation(pcomp->m_PhysicsWorldRotation);
                }

                pcomp->setTransform(t);
            }

        }
            tcomp->externally_modified_position = false;
            tcomp->externally_modified_rotation = false;
            tcomp->externally_modified_scale = false;

        for (auto &e : mChildrenEntities)
        {
            // e->Update();
        }
    }

    void EntityBase::Render()
    {
        for (auto &c : ComponentList)
        {
            c.second->Render();
        }

        for (auto &e : mChildrenEntities)
        {
            // e->Render();
        }
    }

    void EntityBase::PostUpdate()
    {
        /*
        auto tcomp = std::static_pointer_cast<Component::TransformComponent>(ComponentList[Component::TRANSFORMCOMPONENT]);
        tcomp->externally_modified_position = false;
        tcomp->externally_modified_scale = false;
        tcomp->externally_modified_rotation = false;*/
    }
} // namespace Epsilon