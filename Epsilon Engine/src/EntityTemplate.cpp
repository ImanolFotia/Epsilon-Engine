#include <EntityTemplate.h>

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
    for(unsigned int i = 0; i < ComponentList.size(); ++i)
    {
        //if(ComponentList.at(i)->updateIfOutOfView)
            ComponentList.at(i)->Update(resourceManager);
    }
}

void EntityTemplate::Render()
{
    for(unsigned int i = 0; i < ComponentList.size(); ++i){
            if(ComponentList.at(i)->Type == Component::COMPONENT_TYPE::MODELCOMPONENT)
                if(ComponentList.at(i)->isDoubleFaced)
                    glDisable(GL_CULL_FACE);
                else
                    glEnable(GL_CULL_FACE);

        ComponentList.at(i)->Render(resourceManager, this->getPosition());
    }
}

