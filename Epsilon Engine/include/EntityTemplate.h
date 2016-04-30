#pragma once

#include <Component.h>
#include <iostream>
#include <typeinfo>
#include <memory>
#include <ResourceManager.h>

class EntityTemplate
{
public:
    EntityTemplate(std::shared_ptr<ResourceManager> rm);

    ~EntityTemplate()
    {
        std::cout << "Entity Destructor" << std::endl;
    }

    void addComponent(std::shared_ptr<Component::Component> t)
    {
        if(t->Type == Component::MODELCOMPONENT)
            hasModel = true;
        else if(t->Type == Component::SPATIALCOMPONENT)
            hasSpatialComponent = true;
        else if(t->Type == Component::PLAYERCOMPONENT)
            hasPlayerComponent = true;

        ComponentList.push_back(std::move(t));
    }

    void Update();

public:
    bool hasPlayerComponent = false;
    bool hasModel = false;
    bool hasSpatialComponent = false;

    std::string modelPath;
    std::vector<std::shared_ptr<Component::Component>> ComponentList;

private:

    std::shared_ptr<ResourceManager> resourceManager;

protected:

};


