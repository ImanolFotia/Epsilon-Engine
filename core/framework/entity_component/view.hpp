#pragma once

#include <tuple>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <variant>

namespace framework {
    template <typename... TArgs>
    class View
    {
    public:
        class Iterator
        {
        public:
            Iterator(std::tuple<TArgs* ...> v) : values(v)
            {
            }

            Iterator operator++()
            { /*++values;*/
                return *this;
            }
            bool operator!=(Iterator other) { return (values != other.values); }

            std::tuple<TArgs* ...> operator*() { return values; }

        private:
            std::tuple<TArgs *...> values;
        };

        View() = default;

        template <typename T>
        void append(const T& val) {
            std::cout << val->x << std::endl;
            std::get<T>(values) = val;
            std::cout << std::get<T>(values)->x << std::endl;
        }

        template <typename T>
        std::tuple<T> get() {
            return std::get<0>(std::tuple<decltype(get<T>())>(get<T>()));
        }


        Iterator begin() { return Iterator(values); }

        Iterator end()
        {
            return Iterator(values);
        }

    public:
        std::tuple<TArgs* ...> values;
        std::size_t mSize = 1;
        std::unordered_map<std::type_index, int> index;
    };
}