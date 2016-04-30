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

namespace Component
{

enum COMPONENT_TYPE
{
    MODELCOMPONENT = 0,
    SPATIALCOMPONENT,
    PLAYERCOMPONENT,
    SOUNDCOMPONENT,
    PHYSICCOMPONENT
};

class Component
{
public:
    Component() {}
    virtual ~Component() {}
    virtual void Update(){}

    COMPONENT_TYPE Type;
};

/// NPC/Player Components
class PlayerComponent : public Component
{
public:
    PlayerComponent() {
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
    void Update() {}
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

    virtual ~RenderComponent()
    {

        std::cout << "RenderComponent Destructor" << std::endl;
    }
public:

    void Fill(std::string path, std::shared_ptr<ResourceManager>& rm, std::string shader, glm::vec3 pos, glm::vec3 sc, glm::quat rot)
    {
        modelPath = path;
        Position = pos;
        Scale = sc;
        Rotation = rot;
        rm->requestModel(modelPath, rm, Position, Scale, Rotation);
        shaderType = shader;
    }

    void Update(std::shared_ptr<ResourceManager> rm)
    {
        //rm->useModel(modelPath, rm->useShader(shaderType));
    }

    bool hasModel = false;
    std::string modelPath;
    std::string shaderType;

    glm::vec3 Position;
    glm::vec3 Scale;
    glm::quat Rotation;

};

/// Spatial Components
class SpatialComponent : public Component
{
public:
    SpatialComponent() {
        Type = SPATIALCOMPONENT;
    }
    ~SpatialComponent()
    {
        std::cout << "Spatial Destructor" << std:: endl;
    }
    void Fill(float mass, glm::vec3 position)
    {
        Mass = mass;
        Position = position;
    }
    float Mass;
    glm::vec3 Position;
    void Update()
    {
        std::cout << "this is the spatial component" << std::endl;
    }
};
}

#endif // COMPONENT_H_INCLUDED
