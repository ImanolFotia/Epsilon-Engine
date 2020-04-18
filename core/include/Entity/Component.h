///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <ResourceManager.h>
#include <memory>
#include <Types.h>

namespace Component
{

class Component
{
public:
    Component() {}
    virtual ~Component() {
        std::cout << "Component Destroyed" << std::endl;
    }

    virtual void Update() = 0;

    virtual void Render() = 0;

    virtual glm::vec3 getPosition() = 0;
    virtual glm::quat getRotation() = 0;
    virtual glm::vec3 getScale() = 0;

    virtual COMPONENT_TYPE getType() = 0;
};

typedef std::shared_ptr<Component> Component_ptr;

} // namespace Component

