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
#include <Entity/RenderComponent.hpp>
#include <Entity/SoundComponent.hpp>
#include <Entity/ScriptComponent.hpp>

namespace Epsilon { 
class EntityBase : public std::enable_shared_from_this<EntityBase>
{

    using Component_ptr = std::shared_ptr<Component::Component>;

public:
    EntityBase(glm::vec3 pos, glm::vec3 sc, glm::quat rot);

    std::shared_ptr<EntityBase> addComponent(Component_ptr t)
    {
        switch (t->getType())
        {
        case Component::RENDERCOMPONENT:
            mHasRenderComponent = true;
            break;
        case Component::PHYSICCOMPONENT:
            mHasPhysicComponent = true;
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
        case Component::SCRIPTCOMPONENT:
            mHasScriptComponent = true;
            std::static_pointer_cast<Component::ScriptComponent>(t)->setParent(this);
            break;
        default:
            break;
        }

        ComponentList[t->getType()] = t;
        this->mID = ComponentList.size();

        //mChildrenEntities

        //mCollInfo.setName("Entity_" + std::string(Helpers::to_hex(toHash())));

        return shared_from_this();
    }
    std::shared_ptr<EntityBase> addEntity(std::shared_ptr<EntityBase> t)
    {
        
        mChildrenEntities.push_back(t);

        return shared_from_this();
    }

    ~EntityBase()
    {
    }

    void Update();
    void Render();

    void RenderShadows()
    {
        std::cout << "begin render shadows" << std::endl;
        std::static_pointer_cast<Component::RenderComponent>(ComponentList.at(Component::RENDERCOMPONENT))->RenderShadows(); //TODO: Static cast to render component
        std::cout << "end render shadows" << std::endl;
    }

    bool HasRenderComponent() { return mHasRenderComponent; }
    bool HasPhysicComponent() { return mHasPhysicComponent; }
    bool HasClothComponent() { return mHasClothComponent; }
    bool HasMovementComponent() { return mHasMovementComponent; }
    bool HasSoundComponent() { return mHasSoundComponent; }
    bool HasScriptComponent() { return mHasScriptComponent; }

    Component::Component_ptr getComponent(Component::COMPONENT_TYPE type) noexcept
    {
        return ComponentList[type];
    }

    template <class T>
    T getComponent()
    {
        return 0;
    }

private:
    bool mHasRenderComponent = false;
    bool mHasPhysicComponent = false;
    bool mHasClothComponent = false;
    bool mHasMovementComponent = false;
    bool mHasSoundComponent = false;
    bool mHasScriptComponent = false;

    int mID;
    std::string modelPath;
    std::unordered_map<uint8_t, Component_ptr> ComponentList;
    std::vector<std::shared_ptr<EntityBase>> mChildrenEntities;

public:
    glm::vec3 getPosition()
    {
        if (mHasPhysicComponent)
        {
            glm::vec3 v3 = std::static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->getPosition();
            std::static_pointer_cast<Component::RenderComponent>(ComponentList[Component::RENDERCOMPONENT])->setPosition(glm::vec3(v3));
            return v3;
        }
        else if (mHasRenderComponent)
        {
            return std::static_pointer_cast<Component::RenderComponent>(ComponentList[Component::RENDERCOMPONENT])->getPosition();
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
            btVector3 v3 = std::static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_LastPhysicsWorldPosition;
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
            btQuaternion q = std::static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_PhysicsWorldRotation;
            glm::quat rot = glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
            return rot;
        }
        else
            return m_Rotation;
    }

    glm::quat getPrevRotation()
    {
        if (mHasPhysicComponent)
        {
            btQuaternion q = std::static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_LastPhysicsWorldRotation;
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
        {/*
            for (unsigned int i = 0; i < ComponentList.size(); ++i)
            {
                if (ComponentList.at(i)->getType() == Component::RENDERCOMPONENT)
                {
                    return ResourceManager::Get().getModelBoundingBox(std::static_pointer_cast<Component::RenderComponent>(ComponentList[i])->modelPath);
                }
            }*/

            return ResourceManager::Get().getModelBoundingBox(
                    std::static_pointer_cast<Component::RenderComponent> (ComponentList[Component::RENDERCOMPONENT])->modelPath
                );
        }

        else if (this->mHasClothComponent)
        {
           /* for (unsigned int i = 0; i < ComponentList.size(); ++i)
            {
                if (ComponentList.at(i)->getType() == Component::CLOTHCOMPONENT)
                {*/
                    btSoftBody *cloth = std::static_pointer_cast<Physics::ClothPhysicObject>(std::static_pointer_cast<Component::ClothComponent>(ComponentList[Component::CLOTHCOMPONENT])->SoftBodyPointer)->m_BodyCloth.get();

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
               /* }
                continue;
            }*/
        }

        MIN_MAX_POINTS BB = {};
        return BB;
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

protected:
};

using EntityBase_ptr = std::shared_ptr<EntityBase>;

template <>
inline Component::RenderComponent_ptr EntityBase::getComponent()
{
    return std::static_pointer_cast<Component::RenderComponent>(ComponentList[Component::RENDERCOMPONENT]);
}

template <>
inline Component::PhysicComponent_ptr EntityBase::getComponent()
{
    return std::static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT]);
}

template <>
inline Component::SoundComponent_ptr EntityBase::getComponent()
{
    return std::static_pointer_cast<Component::SoundComponent>(ComponentList[Component::SOUNDCOMPONENT]);
}
}