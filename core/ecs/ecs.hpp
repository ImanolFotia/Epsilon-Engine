#pragma once

#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace Epsilon::ecs
{

    class Entity
    {
    public:
        using type = uint32_t;

        Entity(int id) : m_Id(id) {}

        type id() const { return m_Id; }

    private:
        type m_Id;
    };

    class ComponentSystemBase
    {
    public:
        void removeComponent(const Entity &entity)
        {
            if (contains(entity))
            {
                auto index = m_Indices.at(entity.id());
                m_FreeEntities.push(m_Indices.at(entity.id()));
                m_Indices.erase(entity.id());
            }
        }

        const std::vector<Entity> &getEntities() { return m_Entities; }

        uint32_t numEntities() { return m_Entities.size() - m_FreeEntities.size(); }

    protected:
        bool contains(const Entity &entity)
        {
            return m_Indices.contains(entity.id());
        }

    protected:
        std::vector<Entity> m_Entities;
        std::stack<int> m_FreeEntities;
        std::unordered_map<Entity::type, uint32_t> m_Indices;
    };

    template <typename t_ComponentType>
    class ComponentSystem : public ComponentSystemBase
    {
    public:
        ComponentSystem() = default;

        template <class... Args>
        uint32_t emplaceComponent(Entity entity, Args &&...args)
        {
            if (!contains(entity))
            {
                if (m_Components.size() != m_Entities.size())
                    throw std::runtime_error(
                        "Components and entities count missmatch");
            }

            if (m_FreeEntities.size() > 0)
            {
                m_Indices[entity.id()] = m_FreeEntities.top();
                m_FreeEntities.pop();
                m_Entities[m_Indices[entity.id()]] = (entity);
                m_Components[m_Indices[entity.id()]] = t_ComponentType(args...);
            }
            else
            {
                m_Entities.push_back(entity);
                m_Indices[entity.id()] = m_Components.size();

                m_Components.emplace_back(args...);
            }

            return m_Indices[entity.id()];
        }

        template <class... Args>
        uint32_t insertComponent(Entity entity, const t_ComponentType &comp)
        {
            if (!contains(entity))
            {
                if (m_Components.size() != m_Entities.size())
                    throw std::runtime_error(
                        "Components and entities count missmatch");
            }

            if (m_FreeEntities.size() > 0)
            {
                m_Indices[entity.id()] = m_FreeEntities.top();
                m_FreeEntities.pop();
                m_Entities[m_Indices[entity.id()]] = (entity);
                m_Components[m_Indices[entity.id()]] = std::move(comp);
            }
            else
            {
                m_Entities.push_back(entity);
                m_Indices[entity.id()] = m_Components.size();

                m_Components.push_back(std::move(comp));
            }

            return m_Indices[entity.id()];
        }

        template <typename Function, typename... Args>
        void mem_func(Function &&func, Args &&...args)
        {
            for (auto &i : m_Indices)
            {
                auto &c = m_Components.at(i.second);
                std::bind(func, &c, args...)();
            }
        }

        template <typename Function, typename... Args>
        void callback(Function &&func, Args &&...args)
        {
            for (auto &i : m_Indices)
            {
                auto &c = m_Components.at(i.second);
                func(c, args...);
            }
        }

        const t_ComponentType &at(Entity entity)
        {
            return m_Components.at(m_Indices[entity.id()]);
        }

    private:
        std::vector<t_ComponentType> m_Components;
    };

    class EntitySystem
    {
    public:
        [[nodiscard]] Entity create()
        {
            m_Entities.emplace_back(m_EntityCount++);
            return m_Entities.back();
        }

        template <typename component_type, class... Args>
        void emplaceComponent(const Entity &entity, Args... comp)
        {
            using d_type = ComponentSystem<component_type>;
            uint32_t system_index = 0;

            if (!exists<component_type>())
            {
                mComponentIndices[std::type_index(typeid(component_type))] =
                    m_Pools.size();
                m_Pools.push_back(std::make_unique<d_type>());
            }

            system_index = getIndex<component_type>();
            auto *component_system =
                static_cast<d_type *>(m_Pools[system_index].get());
            component_system->emplaceComponent(entity, comp...);
        }

        template <typename component_type>
        void insertComponent(const Entity &entity, const component_type &comp)
        {
            using d_type = ComponentSystem<component_type>;
            uint32_t system_index = 0;

            if (!exists<component_type>())
            {
                mComponentIndices[std::type_index(typeid(component_type))] =
                    m_Pools.size();
                m_Pools.push_back(std::make_unique<d_type>());
            }

            system_index = getIndex<component_type>();
            auto *component_system =
                static_cast<d_type *>(m_Pools[system_index].get());
            component_system->insertComponent(entity, comp);
        }

        template <typename component_type>
        [[nodiscard]] const component_type &getComponent(Entity ent)
        {
            using d_type = ComponentSystem<component_type>;
            auto system_index = getIndex<component_type>();
            auto component_system =
                static_cast<d_type *>(m_Pools[system_index].get());
            return component_system->at(ent);
        }

        template <typename component_type>
        [[nodiscard]] const auto &get()
        {
            using d_type = ComponentSystem<component_type>;
            auto system_index = getIndex<component_type>();
            auto component_system =
                static_cast<d_type *>(m_Pools[system_index].get());
            return component_system->getEntities();
        }

        template <typename component_type>
        void removeComponent(Entity ent)
        {
            using d_type = ComponentSystem<component_type>;
            auto system_index = getIndex<component_type>();
            auto component_system =
                static_cast<d_type *>(m_Pools[system_index].get());
            component_system->removeComponent(ent);
        }

        template <typename component_type>
        [[nodiscard]] std::unique_ptr<ComponentSystem<component_type>> getSystem()
        {
            auto system_index = getIndex<component_type>();
            return m_Pools.at(system_index);
        }

        template <typename component_type, typename Function, typename... Args>
        void mem_func(Function &&func, Args &&...args)
        {
            using d_type = ComponentSystem<component_type>;
            auto system_index = getIndex<component_type>();
            auto component_system =
                static_cast<d_type *>(m_Pools[system_index].get());
            component_system->mem_func(func, args...);
        }

        template <typename component_type, typename Function, typename... Args>
        void callback(Function &&func, Args &&...args)
        {
            auto system_index = getIndex<component_type>();
            auto component_system = static_cast<ComponentSystem<component_type> *>(
                m_Pools[system_index].get());
            component_system->callback(func, args...);
        }

        void destroy() {}

    private:
        template <typename Component>
        constexpr bool exists()
        {
            using component_type = std::decay_t<Component>;
            return mComponentIndices.contains(
                std::type_index(typeid(component_type)));
        }

        template <typename Component>
        constexpr uint32_t getIndex()
        {
            using component_type = std::decay_t<Component>;
            return mComponentIndices[std::type_index(typeid(component_type))];
        }

        int m_EntityCount = 0;
        std::vector<Entity> m_Entities;
        std::vector<std::unique_ptr<ComponentSystemBase>> m_Pools;
        std::unordered_map<std::type_index, uint32_t> mComponentIndices;
    };
}