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
namespace Component
{
/// NPC/Player Components
class PlayerComponent
{
public:
    PlayerComponent() {}
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
class RenderComponent
{
public:
    RenderComponent()
    {

    }

    ~RenderComponent()
    {
/*
        delete model->mesh;
        model->mesh = nullptr;

        delete model;
        model = nullptr;*/
        std::cout << "RenderComponent Destructor" << std::endl;
    }
    void Fill(bool HasModel)
    {
        hasModel = HasModel;
       // Model* model = new Model("models/plant.obj");
        //delete model->mesh;
    }
    bool hasModel;
    void Update()
    {
    }
};

/// Spatial Components
class SpatialComponent
{
public:
    SpatialComponent() {}
    ~SpatialComponent(){ std::cout << "Spatial Destructor" << std:: endl;}
    void Fill(bool mass, glm::vec3 position)
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
