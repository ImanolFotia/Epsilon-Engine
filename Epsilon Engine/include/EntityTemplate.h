#pragma once

#include <Component.h>
#include <iostream>
#include <glm/glm.hpp>
#include <typeinfo>
#include <memory>
#include <ResourceManager.h>

class EntityTemplate
{
public:
    EntityTemplate(std::shared_ptr<ResourceManager> rm, glm::vec3 pos, glm::vec3 sc, glm::quat rot);

    ~EntityTemplate()
    {
       //std::cout << "Entity Destructor" << std::endl;
    }

    void addComponent(std::shared_ptr<Component::Component> t)
    {
        if(t->Type == Component::MODELCOMPONENT){
            hasModel = true;
            this->modelPath = (static_pointer_cast<Component::RenderComponent>(t))->modelPath;
        }
        else if(t->Type == Component::PHYSICCOMPONENT)
        {
            hasPhysicComponent = true;
            m_Position = glm::vec3(t->m_PhysicsWorldPosition.getX(), t->m_PhysicsWorldPosition.getY(),t->m_PhysicsWorldPosition.getZ());

            t->setUserPointer(&CollInfo);
        }
        else if(t->Type == Component::PLAYERCOMPONENT)
            hasPlayerComponent = true;
        else
            std::cout << " not added component" << std::endl;

        ComponentList.push_back(std::move(t));
        this->ID = ComponentList.size();

        CollInfo.setName("Entity_" + Helpers::floatTostring(m_Position.x));
    }

    void Update();

public:
    bool hasPlayerComponent = false;
    bool hasModel = false;
    bool hasPhysicComponent = false;
    int ID;
    std::string modelPath;
    std::vector<std::shared_ptr<Component::Component>> ComponentList;

public:
    glm::vec3 getPosition()
    {
        if(hasPhysicComponent)
        {
            for(int i = 0; i < ComponentList.size(); ++i)
            {
                if(ComponentList.at(i)->Type == Component::PHYSICCOMPONENT)
                    return glm::vec3(ComponentList.at(i)->m_PhysicsWorldPosition.getX(),
                                     ComponentList.at(i)->m_PhysicsWorldPosition.getY(),
                                     ComponentList.at(i)->m_PhysicsWorldPosition.getZ());
            }
        }
        else
        return m_Position;
    }

    glm::vec3 getScale()
    {
        return m_Scale;
    }

    glm::quat getRotation()
    {
        if(hasPhysicComponent)
        {
            for(int i = 0; i < ComponentList.size(); ++i)
            {
                if(ComponentList.at(i)->Type == Component::PHYSICCOMPONENT)
                    return glm::quat(ComponentList.at(i)->m_PhysicsWorldRotation.getW(),
                                     ComponentList.at(i)->m_PhysicsWorldRotation.getX(),
                                     ComponentList.at(i)->m_PhysicsWorldRotation.getY(),
                                     ComponentList.at(i)->m_PhysicsWorldRotation.getZ());
            }
        }
        else
        return m_Rotation;
    }

    MIN_MAX_POINTS getBoundingBox()
    {
        return resourceManager->getModelBoundingBox(modelPath);
    }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Scale;
    glm::quat m_Rotation;
    Physics::CollisionInfo CollInfo;

    std::shared_ptr<ResourceManager> resourceManager;

protected:

};


