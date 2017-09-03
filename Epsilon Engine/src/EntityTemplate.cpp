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
    for(int i = 0; i < ComponentList.size(); ++i)
    {
        ComponentList.at(i)->Update(resourceManager);
    }
}

void EntityTemplate::Render()
{
    for(int i = 0; i < ComponentList.size(); ++i)
        ComponentList.at(i)->Render(resourceManager, this->getPosition());
}

