#pragma once

#include "../WindowBase.hpp"

namespace Epsilon
{
    namespace Windows
    {
        class Window : public WindowBase
        {
        public:
            Window() = default;
            ~Window() {}

            virtual void Create(int w, int h) override
            {
            }
            virtual void Resize(int w, int h) override
            {
            }
            virtual void Destroy() override
            {
            }
        }
    } // namespace Windows
} // namespace Epsilon