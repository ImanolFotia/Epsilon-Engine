#pragma once 

namespace Epsilon {
    template <class T>
    class Container {
        public:
            Container() = default;
            virtual ~Container() {}

            void push_back(T) = 0;
            void pop_back(T) = 0;

            size_t begin() = 0;
            size_t end() = 0;

            void clean() = 0;

            void allocate() = 0;
            void resize() = 0;

            size_t size() = 0;

        protected:
            T* data;
    };
}