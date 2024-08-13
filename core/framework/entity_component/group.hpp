#pragma once

#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <tuple>
#include <stdexcept>

#include "manager.hpp"
#include "list.hpp"
namespace framework {
    class Group {
    public:
        Group() = default;

        template <typename T>
        [[maybe_unused]] void set(List<T>& lst) noexcept {
            mLists[std::type_index(typeid(T))] = &lst;
        }

        template <typename ...TArgs>
        [[maybe_unused]] decltype(auto) get() {
            if constexpr (sizeof...(TArgs) == 0) {
                return std::tuple<TArgs...>({});
            }
            return std::tuple<decltype(*p_get<TArgs>())...>{*p_get<TArgs>()...};
        }
    private:
        template<typename T>
        [[maybe_unused]] decltype(auto) p_get() {
            if (!_exists<T>()) {
                throw std::runtime_error("Trying to cast to unavailable type");
            }
            return reinterpret_cast<List<T>*>(mLists[std::type_index(typeid(T))]);
        }

        template<typename T>
        bool _exists() {
            return mLists.contains(std::type_index(typeid(T)));
        }

        std::unordered_map<std::type_index, void*> mLists;
    };
}
