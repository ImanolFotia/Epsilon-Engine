#pragma once

#include <iostream>

namespace Epsilon
{
    namespace Component
    {
        class IComponent
        {
        public:
            IComponent() = default;

            virtual void Update() = 0;
            virtual void Render() = 0;
        };
        class Mesh : public IComponent
        {
        public:
            Mesh() = default;

            void Update() override { std::cout << "Hello, I'm a Mesh component!" << std::endl; }
            void Render() override {}
        };
        class Script : public IComponent
        {
        public:
            Script() = default;

            void Update() override { std::cout << "Hello, I'm a Script component!" << std::endl; }
            void Render() override {}
        };
    } // namespace Component
} // namespace Epsilon