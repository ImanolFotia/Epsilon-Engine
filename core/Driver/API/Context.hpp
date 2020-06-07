#pragma once

#include <GLFW/glfw3.h>

#include <memory>
#include "Driver/Platform/WindowHandle.hpp"

namespace Epsilon
{
    namespace Platform
    {
        namespace API
        {
            enum CONTEXT_TYPE
            {
                OGL = 0,
                D3D,
                VULKAN,
                METAL
            };
            class ContextBase
            {

            public:
                virtual void Init(std::shared_ptr<WindowHandle<>>, CONTEXT_TYPE) = 0;
                virtual void SwapBuffers() = 0;
                virtual void Shutdown() = 0;
                virtual CONTEXT_TYPE getType()
                {
                    return mType;
                }

            protected:
                std::shared_ptr<WindowHandle<>> mWindowHandle;
                CONTEXT_TYPE mType;
            };

            using ContextBase_ptr = std::shared_ptr<ContextBase>;
        } // namespace API
    }     // namespace Platform
} // namespace Epsilon