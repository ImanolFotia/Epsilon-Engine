#pragma once

#include "API/Context.hpp"
#include "Device.hpp"
#include "WindowHandle.hpp"

#include <memory>

namespace Epsilon
{
    namespace Platform
    {
        class WindowBase
        {

            class WindowData
            {
                int Width;
                int Height;
            };

        public:
            WindowBase() = default;

            virtual ~WindowBase() {}

            virtual void Init(const char*, int, int) const = 0;
            virtual void Resize(int, int) const = 0;
            virtual void Destroy() = 0;

            const API::ContextBase_ptr getContext()
            {
                return mContext;
            }

        protected:
            API::ContextBase_ptr mContext;
        };
    } // namespace Platform
} // namespace Epsilon