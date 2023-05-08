#pragma once

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <map>
#include <algorithm>

namespace framework {
    class ManagerBase
    {
    public:
        virtual void Sort() = 0;
        virtual const std::vector<Entity>& GetEntities() = 0;
        virtual bool Contains(const Entity& entity) const = 0;
    };

    template <typename T>
    class ComponentManager : public ManagerBase
    {
    public:
        ComponentManager()
        {
            mComponents.reserve(1);
            mEntities.reserve(1);
        }
        ComponentManager(const ComponentManager&) = delete;
        ComponentManager(ComponentManager&&) = delete;
        template <typename... Args>
        T& Add(Entity entity, Args... args)
        {

            if (Contains(entity))
            {
                throw std::runtime_error("Entity already contains this component");
            }

            if (mComponents.size() != mEntities.size())
                throw std::runtime_error("Components and entities count missmatch");
            if (mComponents.size() != mIndices.size())
                throw std::runtime_error("Components and indices count missmatch");

            mIndices[entity.Id()] = mComponents.size();

            mEntities.push_back(entity);
            mComponents.push_back(T(args...));

            return mComponents.back();
        }

        void Delete(Entity entity)
        {
            if (Contains(entity))
            {

                auto tempe = mEntities.back();
                mEntities[mIndices[entity.Id()]] = tempe;

                auto tempc = mComponents.back();
                mComponents[mIndices[entity.Id()]] = tempc;

                mEntities.erase(mEntities.end() - 1);
                mComponents.erase(mComponents.end() - 1);

                mIndices[tempe.Id()] = mIndices[entity.Id()];
                mIndices.erase(entity.Id());


                mEntities.shrink_to_fit();
                mComponents.shrink_to_fit();
            }
        }

        constexpr bool inline Contains(const Entity& entity) const
        {
            return mIndices.contains(entity.Id());
        }

        T& GetComponent(Entity entity) noexcept
        {
            return mComponents[mIndices.at(entity.Id())];
        }

        Entity& GetEntity(uint32_t index)
        {
            return mEntities[index];
        }

        int Count()
        {
            return mComponents.size();
        }

        const T& operator[](size_t index)
        {
            return mComponents[index];
        }

        const std::vector<Entity>& GetEntities()
        {
            return mEntities;
        }

        int EntityCount()
        {
            return mEntities.size();
        }

        std::vector<T>& Data()
        {
            return mComponents;
        }

        void Sort()
        {
            std::sort(mEntities.begin(), mEntities.end());
        }

        std::vector<Entity>::iterator begin()
        {
            return mEntities.begin();
        }

        std::vector<Entity>::iterator end()
        {
            return mEntities.end();
        }

    private:
        std::vector<Entity> mEntities;
        std::vector<T> mComponents;
        std::unordered_map<Entity::type /*Entity*/, uint32_t> mIndices;
    };
}