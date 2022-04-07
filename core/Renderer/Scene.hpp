#pragma once

#include <pch.hpp>

#include "../Structs/Octree.hpp"
#include "../Structs/SceneData.hpp"
#include <ecs/ecs.hpp>
#include <camera.h>

#include <Transform.hpp>

namespace Epsilon::Renderer
{

    struct SceneComponent
    {
        SceneComponent(const std::string &n) : m_Name(n) {}
        std::string m_Name;
    };

    class Scene
    {
    public:
        Scene()
        {
            m_SceneGraph = Structs::Octree<SceneNodeData>(glm::vec3(50.0));
        }

        void addEntity(const std::string &name, Transform t)
        {
            auto ent = m_EntitySystem.create();
            m_EntitySystem.emplaceComponent<SceneComponent>(ent, name);
            m_EntitySystem.insertComponent<Transform>(ent, t);
        }

        void gather()
        {
        }

        void render()
        {
        }

    private:
        Structs::Octree<SceneNodeData> m_SceneGraph;
        Renderer m_Renderer;
        ecs::EntitySystem m_EntitySystem;
        std::vector<Camera> m_Cameras;
    };
}