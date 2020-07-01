#include <Entity/Entity.h>

//make available only the types we are using within the namespace, not the whole namespace
//to reduce typing, and make the code more readable

using Epsilon::Component::PhysicComponent;
using Epsilon::Component::RenderComponent;
using Epsilon::Component::SoundComponent;

using Epsilon::Component::CLOTHCOMPONENT;
using Epsilon::Component::PHYSICCOMPONENT;
using Epsilon::Component::RENDERCOMPONENT;
using Epsilon::Component::SOUNDCOMPONENT;

namespace Epsilon
{
    EntityBase::EntityBase(glm::vec3 pos, glm::vec3 sc, glm::quat rot)
    {
        m_Position = pos;
        m_Scale = sc;
        m_Rotation = rot;
        m_PrevScale = sc;
        m_PrevRotation = rot;
    }

    void EntityBase::Update()
    {

        if (mHasSoundComponent)
        {
            glm::vec3 position = this->getPosition();
            std::static_pointer_cast<SoundComponent>(ComponentList.at(SOUNDCOMPONENT))->setPosition(position);
        }

        for (auto &c : ComponentList)
        {
            //if(ComponentList.at(i)->updateIfOutOfView)
            c.second->Update();
        }

        for(auto &e: mChildrenEntities) 
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
        
        for(auto &e: mChildrenEntities) 
        {
           // e->Render();
        }
    }
} // namespace Epsilon