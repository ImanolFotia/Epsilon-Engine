///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

#include <pch.hpp>
#include <glm/glm.hpp>
#include <ResourceManager.h>
#include <Types.h>

namespace Epsilon
{
    class EntityBase;
    namespace Component
    {

        class Component
        {
        public:
            Component(){}
            virtual ~Component()
            {
                std::cout << "Component Destroyed" << std::endl;
            }

            virtual void Update() = 0;

            virtual void Render() = 0;

            virtual COMPONENT_TYPE getType() = 0;

        protected:
            EntityBase* mParent;
        };

        typedef std::shared_ptr<Component> Component_ptr;

    } // namespace Component

} // namespace Epsilon