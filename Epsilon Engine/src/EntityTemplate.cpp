#include <EntityTemplate.h>

EntityTemplate::EntityTemplate(std::shared_ptr<ResourceManager> rm)
{
    resourceManager = rm;
}


void EntityTemplate::Update()
{
    for(int i = 0; i < ComponentList.size(); ++i)
        ComponentList.at(i)->Update();
}

