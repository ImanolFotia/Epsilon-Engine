#pragma once

#include <IComponent.hpp>

namespace Epsilon {
template <typename T>
class ComponentManager {
    public:
        ComponentManager() = default;
        template<class... Args>
        const T& Add(Entity entity, Args... args) {

            if(Contains(entity)) {
                throw std::runtime_error("Entity already contains this component");
            }

            if(mComponents.size() != mEntities.size()) throw std::runtime_error("Components and entities count missmatch");
            if(mComponents.size() != mIndices.size()) throw std::runtime_error("Components and indices count missmatch");

            mIndices[entity.Id()] = mComponents.size();
            
            mEntities.push_back(entity);
            mComponents.push_back(T(args...));

            return mComponents.back();

        }

        bool Contains(Entity entity) const {
            return mIndices.find(entity.Id()) != mIndices.end();
        }

        const T& GetComponent(Entity entity) {
            return mComponents.at(mIndices.at(entity.Id()));
        }

        const Entity& GetEntity(uint32_t index) {
            return mEntities[index];
        }

        int Count() {
            return mComponents.size();
        }

        const T& operator[](size_t index) {
            return mComponents[index];
        }

        const std::vector<T> & Get() {
            return mComponents;
        }

    private:
        std::vector<T> mComponents;
        std::vector<Entity> mEntities;
        std::unordered_map<uint32_t/*Entity*/, uint32_t> mIndices;
};
}