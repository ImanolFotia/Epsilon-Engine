#include <Entity/Entity.h>

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

    if(mHasSoundComponent) {
        glm::vec3 position = this->getPosition();
        static_pointer_cast<Component::SoundComponent>(ComponentList.at(Component::SOUNDCOMPONENT))->setPosition(position);
    }
    
    for(auto & c: ComponentList)
    {
        //if(ComponentList.at(i)->updateIfOutOfView)
            c.second->Update();
    }
}

void EntityTemplate::Render()
{
    for(auto & c: ComponentList){
            if(c.second->getType() == Component::COMPONENT_TYPE::RENDERCOMPONENT){
                if(static_pointer_cast<Component::RenderComponent>(c.second)->isDoubleFaced() == true)
                    glDisable(GL_CULL_FACE);
                else
                    glEnable(GL_CULL_FACE);
            } else if(c.second->getType() == Component::COMPONENT_TYPE::SOUNDCOMPONENT) {
                continue;
            }

        c.second->Render();
    }
}

