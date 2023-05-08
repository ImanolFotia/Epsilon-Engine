#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <atomic>
#include "group.hpp"

namespace framework {
    class EntityManager
    {
    public:
        EntityManager()
        {
            mEntites_view.reserve(1);
            mEntites_view.resize(1);
        }

        Entity CreateEntity() noexcept
        {
            mEntityCount++;
            Entity entity;
            entity.Id(mEntityCount);

            if (mEntityCount >= mEntites_view.size())
            {
                mEntites_view.resize(mEntites_view.size() * 1.5);
            }

            return entity;
        }

        void Sort() const
        {
            for (auto& c : mComponents)
            {
                c.second->Sort();
            }
        }

        template <typename Component, class... Args>
        Component& AddComponent(const Entity& entity, Args... args) noexcept
        {
            using component_type = std::decay_t<Component>;
            //If it's a new type we create a container for this new type
            if (!exists<component_type>())
            {
                mComponents[std::type_index(typeid(component_type))] = std::make_shared<ComponentManager<component_type>>();
            }

            return std::static_pointer_cast<ComponentManager<component_type>>(mComponents.at(_get_type<component_type>()))->Add(entity, args...);
        }

        template <typename Component>
        void deleteComponent(Entity ent) {

            using component_type = std::decay_t<Component>;
            if (exists<component_type>())
            {
                std::static_pointer_cast<ComponentManager<component_type>>(mComponents.at(_get_type<component_type>()))->Delete(ent);
            }

        }

        template <typename Component>
        std::vector<Component>& get() noexcept
        {
            using component_type = std::decay_t<Component>;
            if (!exists<component_type>())
            {
                mComponents[std::type_index(typeid(component_type))] = std::make_shared<ComponentManager<component_type>>();
            }

            return std::static_pointer_cast<ComponentManager<component_type>>(mComponents.at(_get_type<component_type>()))->Data();
        }

        template <typename Component>
        Component& get(Entity entity) noexcept
        {
            using component_type = std::decay_t<Component>;
            if (!exists<component_type>())
            {
                mComponents[std::type_index(typeid(component_type))] = std::make_shared<ComponentManager<component_type>>();
            }

            return std::static_pointer_cast<ComponentManager<component_type>>(mComponents.at(_get_type<component_type>()))->GetComponent(entity);
        }

        template <typename... TArgs>
        [[maybe_unused]] auto view() noexcept -> std::vector<Entity>&
        {
            [[unlikely]] if (!(exists<TArgs>() && ...)) return mEntites_view;

            const auto& elements = { std::tuple<std::type_index, Entity::type>{_get_type<TArgs>(), numEntities<TArgs>()}... };

            std::tuple<std::type_index, Entity::type> min = *elements.begin();

            for (const auto& e : elements)
            {
                min = (std::get<1>(min) > std::get<1>(e)) ? e : min;
            }

            std::atomic<int> index = 0;
            const auto pool = mComponents[std::get<0>(min)].get();

            mEntites_view.resize(pool->GetEntities().size());
#pragma omp parallel for
            for (const auto& ent : pool->GetEntities())
            {
                [[likely]] if ((contains<TArgs>(ent) && ...))
                {
                    mEntites_view[index++] = ent;
                }
            }

            return mEntites_view;
        }

        template <typename Component>
        bool constexpr inline contains(const Entity& ent) noexcept
        {
            using component_type = std::decay_t<Component>;
            return mComponents[_get_type<component_type>()]->Contains(ent);
        }

    private:
        template <typename Component>
        constexpr inline auto _get_type()
        {
            using component_type = std::decay_t<Component>;
            return std::type_index(typeid(component_type));
        }

        template <typename Component>
        constexpr bool exists()
        {
            using component_type = std::decay_t<Component>;
            return mComponents.contains(std::type_index(typeid(component_type)));
        }

        template <typename Component>
        Entity::type numEntities()
        {
            using component_type = std::decay_t<Component>;
            return std::static_pointer_cast<ComponentManager<component_type>>(mComponents[_get_type<component_type>()])->EntityCount();
        }

        std::unordered_map<std::type_index, std::shared_ptr<ManagerBase>> mComponents;

        Entity::type mEntityCount = 0;
        //std::vector<Entity> mEntites;
        std::vector<Entity> mEntites_view;
    };
}