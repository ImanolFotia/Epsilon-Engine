#include <Entity/Entity.h>

//make available only the types we are using within the namespace, not the whole namespace
//to reduce typing, and make the code more readable

using ::Component::SoundComponent;
using ::Component::RenderComponent;
using ::Component::PhysicComponent;

using ::Component::SoundComponent_ptr;
using ::Component::RenderComponent_ptr;
using ::Component::PhysicComponent_ptr;

using ::Component::SOUNDCOMPONENT;
using ::Component::RENDERCOMPONENT;
using ::Component::PHYSICCOMPONENT;

EntityTemplate::EntityTemplate(std::shared_ptr<ResourceManager> rm, glm::vec3 pos, glm::vec3 sc, glm::quat rot)
{
    resourceManager = rm;
    m_Position = pos;
    m_Scale = sc;
    m_Rotation = rot;
    m_PrevScale = sc;
    m_PrevRotation = rot;
}

void EntityTemplate::Update()
{

    if (mHasSoundComponent)
    {
        glm::vec3 position = this->getPosition();
        static_pointer_cast<SoundComponent>(ComponentList.at(SOUNDCOMPONENT))->setPosition(position);
    }

    for (auto &c : ComponentList)
    {
        //if(ComponentList.at(i)->updateIfOutOfView)
        c.second->Update();
    }
}

void EntityTemplate::Render()
{
    for (auto &c : ComponentList)
    {
        if (c.second->getType() == RENDERCOMPONENT)
        {
            if (static_pointer_cast<RenderComponent>(c.second)->isDoubleFaced() == true)
                glDisable(GL_CULL_FACE);
            else
                glEnable(GL_CULL_FACE);
        }

        c.second->Render();
    }
}

template <>
inline RenderComponent_ptr EntityTemplate::getComponent()
{
    return std::static_pointer_cast<RenderComponent>(ComponentList[RENDERCOMPONENT]);
} 

template <>
inline PhysicComponent_ptr EntityTemplate::getComponent()
{
    return std::static_pointer_cast<PhysicComponent>(ComponentList[PHYSICCOMPONENT]);
}

template <>
inline SoundComponent_ptr EntityTemplate::getComponent()
{
    return std::static_pointer_cast<SoundComponent>(ComponentList[SOUNDCOMPONENT]);
}
