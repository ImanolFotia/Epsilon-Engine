#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <typeinfo>
#include <memory>
#include <ResourceManager.h>
#include <Physics/CollisionInfo.h>

#include <Entity/ClothComponent.hpp>
#include <Entity/MovementComponent.hpp>
#include <Entity/PhysicsComponent.hpp>
#include <Entity/PlayerComponent.hpp>
#include <Entity/RenderComponent.hpp>
#include <Entity/SoundComponent.hpp>

class EntityTemplate : public std::enable_shared_from_this<EntityTemplate>
{

    using Component_ptr = std::shared_ptr<Component::Component>;

public:
    EntityTemplate(std::shared_ptr<ResourceManager> rm, glm::vec3 pos, glm::vec3 sc, glm::quat rot);

    std::shared_ptr<EntityTemplate> addComponent(Component_ptr t)
    {
        switch (t->getType())
        {
        case Component::RENDERCOMPONENT:
            mHasRenderComponent = true;
            break;
        case Component::PHYSICCOMPONENT:
            mHasPhysicComponent = true;
            break;
        case Component::PLAYERCOMPONENT:
            mHasPlayerComponent = true;
            break;
        case Component::CLOTHCOMPONENT:
            mHasClothComponent = true;
            break;
        case Component::MOVEMENTCOMPONENT:
            mHasMovementComponent = true;
            break;
        case Component::SOUNDCOMPONENT:
            mHasSoundComponent = true;
            break;
        default:
            break;
        }

        ComponentList[t->getType()] = t;
        this->mID = ComponentList.size();

        //mCollInfo.setName("Entity_" + std::string(Helpers::to_hex(toHash())));

        return shared_from_this();
    }

    ~EntityTemplate()
    {
    }

    void Update();
    void Render();

    void RenderShadows()
    {
        std::cout << "begin render shadows" << std::endl;
        static_pointer_cast<Component::RenderComponent>(ComponentList.at(Component::RENDERCOMPONENT))->RenderShadows(); //TODO: Static cast to render component
        std::cout << "end render shadows" << std::endl;
    }

    bool HasPlayerComponent() { return mHasPlayerComponent; }
    bool HasRenderComponent() { return mHasRenderComponent; }
    bool HasPhysicComponent() { return mHasPhysicComponent; }
    bool HasClothComponent() { return mHasClothComponent; }
    bool HasMovementComponent() { return mHasMovementComponent; }
    bool HasSoundComponent() { return mHasSoundComponent; }

    Component_ptr getComponent(Component::COMPONENT_TYPE type) {
        return ComponentList[type];
    }

private:
    bool mHasPlayerComponent = false;
    bool mHasRenderComponent = false;
    bool mHasPhysicComponent = false;
    bool mHasClothComponent = false;
    bool mHasMovementComponent = false;
    bool mHasSoundComponent = false;

    int mID;
    std::string modelPath;
    std::unordered_map<uint8_t, Component_ptr> ComponentList;

public:
    glm::vec3 getPosition()
    {
        if (mHasPhysicComponent)
        {
            btVector3 v3 = static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_PhysicsWorldPosition;
            return glm::vec3(v3.getX(), v3.getY(), v3.getZ());
        }
        else if (mHasRenderComponent)
        {
            return static_pointer_cast<Component::RenderComponent>(ComponentList[Component::RENDERCOMPONENT])->getPosition();
        }
        else
        {
            return m_Position;
        }
    }

    glm::vec3 getPrevPosition()
    {
        if (mHasPhysicComponent)
        {
            btVector3 v3 = static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_LastPhysicsWorldPosition;
            return glm::vec3(v3.getX(), v3.getY(), v3.getZ());
        }
        else
        {
            return m_Position;
        }
    }

    glm::vec3 getScale()
    {
        return m_Scale;
    }

    glm::vec3 getPrevScale()
    {
        return m_PrevScale;
    }

    glm::quat getRotation()
    {
        if (mHasPhysicComponent)
        {
            btQuaternion q = static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_PhysicsWorldRotation;
            return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
        }
        else
            return m_Rotation;
    }

    glm::quat getPrevRotation()
    {
        if (mHasPhysicComponent)
        {
            btQuaternion q = static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_LastPhysicsWorldRotation;
            return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
        }
        else
        {
            return m_PrevRotation;
        }
    }

    MIN_MAX_POINTS getBoundingBox()
    {

        if (this->mHasRenderComponent)
        {
            for (unsigned int i = 0; i < ComponentList.size(); ++i)
            {
                if (ComponentList.at(i)->getType() == Component::RENDERCOMPONENT)
                {
                    return resourceManager->getModelBoundingBox(std::static_pointer_cast<Component::RenderComponent>(ComponentList[i])->modelPath);
                }
            }
        }
        else if (this->mHasClothComponent)
        {
            for (unsigned int i = 0; i < ComponentList.size(); ++i)
            {
                if (ComponentList.at(i)->getType() == Component::CLOTHCOMPONENT)
                {
                    btSoftBody *cloth = std::static_pointer_cast<Physics::ClothPhysicObject>(std::static_pointer_cast<Component::ClothComponent>(ComponentList[i])->RigidBodyPointer)->m_BodyCloth.get();

                    btVector3 aabbMin, aabbMax;
                    cloth->getAabb(aabbMin, aabbMax);

                    MIN_MAX_POINTS BB;
                    BB.MAX_X = aabbMax.x();
                    BB.MAX_Y = aabbMax.y();
                    BB.MAX_Z = aabbMax.z();
                    BB.MIN_X = aabbMin.x();
                    BB.MIN_Y = aabbMin.y();
                    BB.MIN_Z = aabbMin.z();

                    return BB;
                }
                continue;
            }
        }
        /*
        BB.MIN_X *= this->getScale().x; BB.MAX_X *= this->getScale().x;
        BB.MIN_Y *= this->getScale().y; BB.MAX_Y *= this->getScale().y;
        BB.MIN_Z *= this->getScale().z; BB.MAX_Z *= this->getScale().z;*/
    }

    long toHash()
    {
        return std::hash<float>{}(m_Position.x + m_Position.y + m_Position.z + mID);
    }

    void setShader(std::string sh)
    {
        if (mHasRenderComponent)
        {
            std::static_pointer_cast<Component::RenderComponent>(ComponentList.at(Component::RENDERCOMPONENT))->setShader(sh);
        }
    }

    std::string getModelPath()
    {
        if (mHasRenderComponent)
        {
            return std::static_pointer_cast<Component::RenderComponent>(ComponentList[Component::RENDERCOMPONENT])->getModelPath();
        }
        return "";
    }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Scale;
    glm::quat m_Rotation;
    glm::vec3 m_PrevPosition;
    glm::vec3 m_PrevScale;
    glm::quat m_PrevRotation;

    std::shared_ptr<ResourceManager> resourceManager;

protected:
};
