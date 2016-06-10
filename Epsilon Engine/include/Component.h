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
    virtual void Update(std::shared_ptr<ResourceManager> rm) = 0;

    COMPONENT_TYPE Type;
    btVector3 m_PhysicsWorldPosition;
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
        m_PhysicsWorldRotation = RigidBodyPointer->Body->getOrientation();
    }
    float Mass;

    std::shared_ptr<Physics::PhysicObject> RigidBodyPointer;

    void Update(std::shared_ptr<ResourceManager> rm)
    {
        m_PhysicsWorldPosition = RigidBodyPointer->Body->getCenterOfMassPosition();
        m_PhysicsWorldRotation = RigidBodyPointer->Body->getOrientation();
    }

    /** Functions declared for the sake of pure virtual function polymorphism, must not be used for production*/
    virtual void Fill(bool, bool){}
    virtual void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader){}
};
}

#endif // COMPONENT_H_INCLUDED
