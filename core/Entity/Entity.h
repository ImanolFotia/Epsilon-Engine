#pragma once

#include <pch.hpp>
#include <glm/glm.hpp>
#include <ResourceManager.h>
#include <Physics/CollisionInfo.h>

#include <Entity/ClothComponent.hpp>
#include <Entity/MovementComponent.hpp>
#include <Entity/PhysicsComponent.hpp>
#include <Entity/RenderComponent.hpp>
#include <Entity/SoundComponent.hpp>
#include <Entity/ScriptComponent.hpp>
#include <Entity/TransformComponent.hpp>

namespace Epsilon
{
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

            using Component::TRANSFORMCOMPONENT;
            Transform = std::static_pointer_cast<Component::TransformComponent>(ComponentList[TRANSFORMCOMPONENT]);
            //mChildrenEntities
            mHash = std::string("0x" + toHash(Transform->Position(), Transform->Scale(), Transform->Rotation()));

            //mCollInfo.setName("Entity_" + std::string(Helpers::to_hex(toHash())));

            return shared_from_this();
        }
        std::shared_ptr<EntityBase> addEntity(std::shared_ptr<EntityBase> t)
        {

            mChildrenEntities.push_back(t);

            return shared_from_this();
        }

        std::string mHash;

        std::string toHash(glm::vec3 pos, glm::vec3 sc, glm::quat rot)
        {
            return std::to_string(mID * pos.x + pos.y * pos.z * pos.z + sc.x - rot.x * rot.z);
        }

        const std::string &getHash()
        {
            return mHash;
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
        std::shared_ptr<Component::TransformComponent> Transform;

        int mID;
        std::string modelPath;
        std::unordered_map<uint8_t, Component_ptr> ComponentList;
        std::vector<std::shared_ptr<EntityBase>> mChildrenEntities;

    public:

        auto & getComponentList() {
            return ComponentList;
        }

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
                return Transform->Position();
            }
        }

        glm::vec3 getPrevPosition()
        {
            if (mHasPhysicComponent)
            {
                btVector3 v3 = std::static_pointer_cast<Component::PhysicComponent>(ComponentList[Component::PHYSICCOMPONENT])->m_LastPhysicsWorldPosition;
                return glm::vec3(v3.getX(), v3.getY(), v3.getZ());
            }
            else if (mHasRenderComponent)
            {
                return std::static_pointer_cast<Component::RenderComponent>(ComponentList[Component::RENDERCOMPONENT])->mPrevPosition;
            }
            else
            {
                return Transform->PrevPosition();
            }
        }

        glm::vec3 getScale()
        {
            return Transform->Scale();
        }

        void setPosition(glm::vec3 pos)
        {
            //if(this->mHasPhysicComponent)
            Transform->PrevPosition(Transform->Position());
            Transform->Position(pos);
        }

        glm::vec3 getPrevScale()
        {
            return Transform->PrevScale();
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
                return Transform->Rotation();
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
                return Transform->PrevRotation();
            }
        }

        MIN_MAX_POINTS getBoundingBox()
        {

            if (this->mHasRenderComponent)
            { /*
            for (unsigned int i = 0; i < ComponentList.size(); ++i)
            {
                if (ComponentList.at(i)->getType() == Component::RENDERCOMPONENT)
                {
                    return ResourceManager::Get().getModelBoundingBox(std::static_pointer_cast<Component::RenderComponent>(ComponentList[i])->modelPath);
                }
            }*/

                return ResourceManager::Get().getModelBoundingBox(
                    std::static_pointer_cast<Component::RenderComponent>(ComponentList[Component::RENDERCOMPONENT])->modelPath);
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

        std::function<void(EntityBase*)> mFunction;

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

    template <>
    inline Component::ClothComponent_ptr EntityBase::getComponent()
    {
        return std::static_pointer_cast<Component::ClothComponent>(ComponentList[Component::CLOTHCOMPONENT]);
    }
}