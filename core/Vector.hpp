#pragma once

namespace Epsilon {
    template<class T>
    class Vector : public Container {

            public:
                Vector() = default;
                Vector(size_t) {
                    allocate()
                }
                
            void push_back(T){

            }
            void pop_back(T){}

            size_t begin(){}
            size_t end(){}

            void clean(){}

            void allocate(){}
            void resize(){}

            size_t size() {}
    };
}