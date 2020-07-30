#pragma once 

#include <pch.hpp>

template <typename... TArgs>
class View {
    public:
        class Iterator {
            public:
                Iterator(std::tuple<TArgs*...> v) : values(v) {
                }

                Iterator operator++() { /*++values;*/ return *this; }
                bool operator!=(Iterator other) { return (values != other.values); }
                std::tuple<TArgs*...> operator*() {return values;}
            private:
                std::tuple<TArgs*...> values;
        };

        View() {
            values = std::make_tuple(&sc, &ms);
            int i = 0;
            std::vector<std::type_index> v;
            v.insert(v.end(), {typeid(TArgs)...});
            index.insert(std::make_pair({typeid(TArgs)...}, ++i));
        }

        Iterator begin() { return Iterator(values); }

        Iterator end() { 
            return Iterator(values); 
        }

    public:
        std::tuple<TArgs*...> values;
        std::size_t mSize = 1;
        std::unordered_map<TArgs..., int> index;
};