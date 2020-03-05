///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED
#include <iostream>
#include <glm/glm.hpp>
#include <Model.h>
#include <ResourceManager.h>
#include <memory>
#include <Types.h>
#include <Physics/Physics.h>
#include <Patch.h>

namespace Component
{
    class Component
    {
    public:
        Component() {}
        virtual ~Component() {}

        virtual void Fill(bool HasHealth, bool HasGun) = 0;
        virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader) = 0;
        virtual void Fill(float mass, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer) = 0;

        virtual void setUserPointer(void *userPointer) {}

        virtual btTransform getTransform(){return btTransform();}

        virtual void setTransform(btTransform){}

        virtual void Update(std::shared_ptr<ResourceManager> rm) = 0;

        virtual void Render(std::shared_ptr<ResourceManager> rm, glm::vec3) = 0;

        virtual COMPONENT_TYPE getType() = 0;

        virtual void RenderShadows(){}

        virtual void setTransparency(bool x) = 0;

        void setShader(string sh){}

        btVector3 m_PhysicsWorldPosition;
        btVector3 m_LastPhysicsWorldPosition;
        btVector3 m_PhysicsWorldScale;
        btQuaternion m_PhysicsWorldRotation;
        btQuaternion m_LastPhysicsWorldRotation;
        bool updateIfOutOfView = true;
        bool isTransparent = false;
        bool isDoubleFaced = false;
    };

/// NPC/Player Components
    class PlayerComponent : public Component
    {
    public:
        PlayerComponent()
        {
            mType = PLAYERCOMPONENT;
        }
        ~PlayerComponent() {}

        void Fill(bool HasHealth, bool HasGun)
        {
            hasHealth = HasHealth;
            hasGun = HasGun;
        }

        COMPONENT_TYPE getType() { return mType;}

        bool hasHealth;
        bool hasGun;
        void Update(std::shared_ptr<ResourceManager> rm) {}

        private:
            COMPONENT_TYPE mType;
    };

/// Rendering Components
    class RenderComponent : public Component
    {
    public:
        RenderComponent()
        {
            Type = MODELCOMPONENT;
            hasModel = true;
        }

        COMPONENT_TYPE getType() { return mType;}
        ~RenderComponent()
        {
            std::cout << "RenderComponent Destructor" << std::endl;
        }
    public:

        void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader)
        {
            modelPath = path;
            rm->requestModel(modelPath, rm, glm::vec3(0), glm::vec3(1), glm::quat(0,0,0,0));
            shaderType = shader;
        }

        void Update(std::shared_ptr<ResourceManager> rm){

        }

        void Render(std::shared_ptr<ResourceManager> rm, glm::vec3 pos)
        {
            glUniform1i(glGetUniformLocation(rm->getShaderID(shaderType), "isTransparent"), this->isTransparent);
            if(isDoubleFaced) glDisable(GL_CULL_FACE);
            rm->useModel(modelPath, rm->getShaderID(shaderType), pos);
            glEnable(GL_CULL_FACE);
        }

        void setShader(std::string sh)
        {
            shaderType = sh;
        }

        void setTransparency(bool x) {isTransparent = x;}

        bool hasModel = false;
        std::string modelPath;
        std::string shaderType;
        MODEL_TYPE ModelType;

        /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
        virtual void Fill(bool, bool) {}
        virtual void Fill(float, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer) {}

    };

/// Physic Components
    class PhysicComponent : public Component
    {
        using BaseShape_ptr = std::shared_ptr<Physics::PhysicObject>;
        using CubeShape_ptr = std::shared_ptr<Physics::CubePhysicObject>;
        using SphereShape_ptr = std::shared_ptr<Physics::SpherePhysicObject>;
        using TriangleMeshShape_ptr = std::shared_ptr<Physics::TriangleMeshPhysicObject>;
        using ClothShape_ptr = std::shared_ptr<Physics::ClothPhysicObject>;
        using DynamycWorld_ptr = std::shared_ptr<btDynamicsWorld>;

    public:
        PhysicComponent(float mass, Type type, DynamycWorld_ptr world) : Type(PHYSICCOMPONENT), mMass(mass)
        {
            BaseShape_ptr _shape;
            switch(type){
                case Type::CUBE:
                    mRigidBodyPointer = make_shared<Physics::CubePhysicObject>();
                break;
                case Type::SPHERE:
                    mRigidBodyPointer = make_shared<Physics::SpherePhysicObject>();
                break;
                case Type::TRIANGLE_MESH:
                    mRigidBodyPointer = make_shared<Physics::TriangleMeshPhysicObject>();
                break;
                case Type::CLOTH:
                    mRigidBodyPointer = make_shared<Physics::ClothPhysicObject>();
                break;
                default:
                    mRigidBodyPointer = make_shared<Physics::SpherePhysicObject>();
                break;
            }

            world->addRigidBody(_shape->addObject(glm::vec3(16, 15, 10), 100.0, rM->getModelBoundingBox("models/shape_mat.eml"), 0.25).get());
            

            m_PhysicsWorldPosition = mRigidBodyPointer->Body->getCenterOfMassPosition();
            m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
            m_PhysicsWorldRotation = mRigidBodyPointer->Body->getOrientation();
            
            btTransform transf = getTransform();
            btQuaternion rot = btQuaternion(m_Rotation.x, m_Rotation.y, m_Rotation.z, m_Rotation.w);
            transf.setRotation(rot);
            setTransform(transf);
            setUserPointer(&CollInfo);
        }
        ~PhysicComponent()
        {
            std::cout << "Physic Destructor" << std:: endl;
        }
        
        COMPONENT_TYPE getType() { return mType;}
        void Fill(float mass, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer)
        {
        }
        
        float mMass;

        void Update(std::shared_ptr<ResourceManager> rm)
        {

            m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
            m_PhysicsWorldPosition = mRigidBodyPointer->Body->getCenterOfMassPosition();

            /*
                    glm::vec3 pos  = glm::mix(glm::vec3(m_LastPhysicsWorldPosition.getX(), m_LastPhysicsWorldPosition.getY(), m_LastPhysicsWorldPosition.getZ()),
                                                 glm::vec3(m_PhysicsWorldPosition.getX(), m_PhysicsWorldPosition.getY(), m_PhysicsWorldPosition.getZ()),
                                                 rm->timestep*60);

                    m_PhysicsWorldPosition = btVector3(pos.x, pos.y, pos.z);*/
            m_LastPhysicsWorldRotation = m_PhysicsWorldRotation;
            m_PhysicsWorldRotation = mRigidBodyPointer->Body->getOrientation();
        }

        virtual void setUserPointer(void* userPointer)
        {
            mRigidBodyPointer->Body->setUserPointer(userPointer);
        }

        virtual btTransform getTransform()
        {
            return mRigidBodyPointer->Body->getCenterOfMassTransform();
        }

        virtual void setTransform(btTransform t)
        {
            mRigidBodyPointer->Body->setCenterOfMassTransform(t);
        }

        /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
        virtual void Fill(bool, bool) {}
        virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader) {}
        void Render(std::shared_ptr<ResourceManager> rm, glm::vec3){}
        virtual void setTransparency(bool x){}
        Physics::CollisionInfo CollInfo;
        BaseShape_ptr mRigidBodyPointer = nullptr;
        
    };

    class MovementComponent : public Component
    {
    public:
        MovementComponent() {
            Type = MOVEMENTCOMPONENT;
        }
        ~MovementComponent() {}

        COMPONENT_TYPE getType() { return mType;}
        void Fill(std::shared_ptr<Physics::PhysicObject> rigidBody, glm::vec3 from, glm::vec3 to, float speed, bool loop)
        {
            RigidBodyPointer = rigidBody;
            m_FromPos = from;
            m_ToPos = to;
            m_Speed = glm::abs(speed);
            m_Loop = loop;
        }

        void Update(std::shared_ptr<ResourceManager> rm)
        {

            m_Interpolation = glm::clamp(m_Rate,0.0f ,1.0f);

            if(m_Loop) {
                if(m_Interpolation >= 1.0)
                    m_Up = false;
                else if(m_Interpolation <= 0.0)
                    m_Up = true;
                else if(m_Interpolation >= 0.0 && m_Interpolation <= 1.0 && !m_Up)
                {
                    m_Up = false;
                }
                else if(m_Interpolation >= 0.0 && m_Interpolation <= 1.0 && m_Up){
                    m_Up = true;
                }
            }

            if(m_Loop)
            {
                if(m_Up)
                    m_Rate += m_Speed * rm->timestep;
                else
                    m_Rate -= m_Speed * rm->timestep;
            }
            else
            {
                m_Rate += m_Speed * rm->timestep;
            }

            m_CurrentPosition = glm::mix(m_FromPos, m_ToPos, m_Interpolation);
            btVector3 deltaPos = btVector3(m_CurrentPosition.x, m_CurrentPosition.y, m_CurrentPosition.z) - RigidBodyPointer->Body->getCenterOfMassPosition();
            RigidBodyPointer->Body->translate(btVector3(deltaPos));
        }


        virtual void Fill(bool, bool) {}
        virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader) {}
        virtual void Fill(float mass, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer) {}
        void Render(std::shared_ptr<ResourceManager> rm){}
        void Render(std::shared_ptr<ResourceManager> rm, glm::vec3){}
        virtual void setTransparency(bool x){}

    private:

        glm::vec3 m_FromPos;
        glm::vec3 m_ToPos;
        glm::vec3 m_CurrentPosition;
        float m_Speed;
        float m_Interpolation = 0.0;
        float m_Rate = 0;
        bool m_Loop;
        bool m_Up = true;
        std::shared_ptr<Physics::PhysicObject> RigidBodyPointer;

    protected:

    };

/// Rendering Components
    class SoundComponent : public Component
    {
    public:
        SoundComponent()
        {
        }

        COMPONENT_TYPE getType() { return mType;}
        ~SoundComponent()
        {
            std::cout << "SoundComponent Destructor" << std::endl;
        }
    public:

        void Fill(float volume, float radius, glm::vec3 position, glm::vec3 direction)
        {

        }

        void Update(std::shared_ptr<ResourceManager> rm)
        {
        }


        /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
        virtual void Fill(bool, bool) {}
        virtual void Fill(float, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer) {}
        virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader) {}
        void Render(){}
        virtual void setTransparency(bool x) {}

    };

    class ClothComponent : public Component
    {
    public:
        ClothComponent(std::shared_ptr<Camera>& inPointerToCamera)
        {
            PointerToCamera = inPointerToCamera;
            updateIfOutOfView = false;
            Type = CLOTHCOMPONENT;
        }

        COMPONENT_TYPE getType() { return mType;}
        ~ClothComponent()
        {
            std::cout << "ClothComponent Destructor" << std::endl;
        }
    public:

        void Fill(std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer)
        {
            std::shared_ptr<Physics::ClothPhysicObject> cloth = std::static_pointer_cast<Physics::ClothPhysicObject>(PhysicBodyPointer);
            mPatch = (std::shared_ptr<Patch>) new Patch(glm::vec3(0.0), cloth->getScale(), cloth->getWidth(), cloth->getHeight(), "sponza/sponza_curtain_diff.tga");
            RigidBodyPointer = PhysicBodyPointer;
        }

        void Render(std::shared_ptr<ResourceManager> rm, glm::vec3 pos)
        {
            mPatch->Render(rm->useShader(shaderType), PointerToCamera->getViewMatrix(), PointerToCamera->getProjectionMatrix());
        }

        virtual void RenderShadows(){
            mPatch->RenderShadows();
        }

        void Update(std::shared_ptr<ResourceManager> rm)
        {
            //std::cout << "Llega Update cloth" <<std::endl;
            std::shared_ptr<Physics::ClothPhysicObject> cloth = std::static_pointer_cast<Physics::ClothPhysicObject>(RigidBodyPointer);
            mPatch->updateVertexBuffers(cloth->getVertices());
        }

        void setShader(std::string sh)
        {
            shaderType = sh;
        }

        std::shared_ptr<Patch> mPatch;
        std::shared_ptr<Physics::PhysicObject> RigidBodyPointer = nullptr;
        std::shared_ptr<Camera> PointerToCamera;
        /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
        virtual void Fill(bool, bool) {}
        virtual void Fill(float, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer) {}
        virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader) {}
        void Render(){}
        virtual void setTransparency(bool x) {}
        std::string shaderType;

    };

}

#endif // COMPONENT_H_INCLUDED
