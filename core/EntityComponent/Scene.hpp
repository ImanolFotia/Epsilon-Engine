#pragma once

#include <pch.hpp>

#include <EntityComponent/Entity.hpp>
#include <ComponentManager.hpp>

namespace Epsilon {
    
class Registry {
    public:
        Registry(){}

        Entity CreateEntity() {
            Entity entity;
            entity.Id(mEntityCount);
            mEntityCount++;

            return entity;
        }

        template<typename T, class... Args>
        T AddComponent(Entity entity, Args... args) {
            //If it's a new type we create a container for this new type
            if(!exists<T>()){
                mComponents[std::type_index(typeid(T))] = ComponentManager<T>();
            }
            
            return std::any_cast<ComponentManager<T>>(&mComponents[std::type_index(typeid(T))])->Add(entity, args...);
        }

        template<typename T>
        ComponentManager<T> Components(){ 
            if(!exists<T>())
            {
                mComponents[std::type_index(typeid(T))] = ComponentManager<T>();
            }
            return *std::any_cast<ComponentManager<T>>(&mComponents[std::type_index(typeid(T))]);
        }

        template<typename T>
        const std::vector<T>& Get(){
            if( !exists<T>() )
            {
                mComponents[std::type_index(typeid(T))] = ComponentManager<T>();
            }
            return std::any_cast<ComponentManager<T>>(&mComponents[std::type_index(typeid(T))]).Get();
        }

        template<typename T, typename U>
        const std::tuple<std::vector<T>, std::vector<U>>& Get(){
            if( !exists<T>()) mComponents[std::type_index(typeid(T))] = ComponentManager<T>(); 
            if( !exists<U>()) mComponents[std::type_index(typeid(U))] = ComponentManager<U>(); 

            return std::make_tuple(
                        std::any_cast<ComponentManager<T>>( &mComponents[std::type_index(typeid(T))] ).Get(),
                        std::any_cast<ComponentManager<T>>( &mComponents[std::type_index(typeid(U))] ).Get()
                        );
        }

    private:

        template<typename T>
        bool exists(){
            return mComponents.find(std::type_index(typeid(T))) != mComponents.end();
        }

        std::unordered_map<std::type_index, std::any> mComponents;

        uint32_t mEntityCount = 0;
};
}