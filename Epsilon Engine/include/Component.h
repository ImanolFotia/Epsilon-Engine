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
#include <Physics.h>

namespace Component
{
class Component
{
public:
    Component() {}
    virtual ~Component(){}

    virtual void Fill(bool HasHealth, bool HasGun) = 0;
    virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader) = 0;
    virtual void Fill(float mass, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer) = 0;

    virtual void setUserPointer(void *userPointer){}

    virtual void Update(std::shared_ptr<ResourceManager> rm) = 0;

    COMPONENT_TYPE Type;
    btVector3 m_PhysicsWorldPosition;
    btVector3 m_LastPhysicsWorldPosition;
    btVector3 m_PhysicsWorldScale;
    btQuaternion m_PhysicsWorldRotation;
};

/// NPC/Player Components
class PlayerComponent : public Component
{
public:
    PlayerComponent()
    {
        Type = PLAYERCOMPONENT;
    }
    ~PlayerComponent() {}
    void Fill(bool HasHealth, bool HasGun)
    {
        hasHealth = HasHealth;
        hasGun = HasGun;
    }
    bool hasHealth;
    bool hasGun;
    void Update(std::shared_ptr<ResourceManager> rm) {}
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

    void Update(std::shared_ptr<ResourceManager> rm)
    {
        rm->useModel(modelPath, rm->getShaderID(shaderType));
    }

    bool hasModel = false;
    std::string modelPath;
    std::string shaderType;
    MODEL_TYPE ModelType;

    /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
    virtual void Fill(bool, bool){}
    virtual void Fill(float, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer){}

};

/// Physic Components
class PhysicComponent : public Component
{
public:
    PhysicComponent()
    {
        Type = PHYSICCOMPONENT;
    }
    ~PhysicComponent()
    {
        std::cout << "Physic Destructor" << std:: endl;
    }
    void Fill(float mass, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer)
    {
        Mass = mass;
        RigidBodyPointer = PhysicBodyPointer;

        m_PhysicsWorldPosition = RigidBodyPointer->Body->getCenterOfMassPosition();
        m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
        m_PhysicsWorldRotation = RigidBodyPointer->Body->getOrientation();
    }
    float Mass;

    std::shared_ptr<Physics::PhysicObject> RigidBodyPointer;

    void Update(std::shared_ptr<ResourceManager> rm)
    {

        m_PhysicsWorldPosition = RigidBodyPointer->Body->getCenterOfMassPosition();
/*
        glm::vec3 pos  = glm::mix(glm::vec3(m_LastPhysicsWorldPosition.getX(), m_LastPhysicsWorldPosition.getY(), m_LastPhysicsWorldPosition.getZ()),
                                     glm::vec3(m_PhysicsWorldPosition.getX(), m_PhysicsWorldPosition.getY(), m_PhysicsWorldPosition.getZ()),
                                     rm->timestep*60);

        m_PhysicsWorldPosition = btVector3(pos.x, pos.y, pos.z);*/
        m_LastPhysicsWorldPosition = m_PhysicsWorldPosition;
        m_PhysicsWorldRotation = RigidBodyPointer->Body->getOrientation();
    }

    virtual void setUserPointer(void* userPointer)
    {
        RigidBodyPointer->Body->setUserPointer(userPointer);
    }

    /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
    virtual void Fill(bool, bool){}
    virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader){}
};



/// Rendering Components
class SoundComponent : public Component
{
public:
    SoundComponent()
    {
    }

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
    virtual void Fill(bool, bool){}
    virtual void Fill(float, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer){}
    virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader){}

};

}

#endif // COMPONENT_H_INCLUDED
