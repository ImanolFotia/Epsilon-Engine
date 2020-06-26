#pragma once
/*
#if defined(EPSILON_STATIC) || defined(__cplusplus_cli)
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#else
#include <GLFW/glfw3.h>
#endif*/

#include <Core.hpp>

#include <memory>
#include "Platform/WindowHandle.hpp"

namespace Epsilon
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
                virtual void Init(std::shared_ptr<Platform::WindowHandle<>>, CONTEXT_TYPE) = 0;
                virtual void SwapBuffers() = 0;
                virtual void Shutdown() = 0;
                virtual CONTEXT_TYPE getType()
                {
                    return mType;
                }

            protected:
                std::shared_ptr<Platform::WindowHandle<>> mWindowHandle;
                CONTEXT_TYPE mType;
            };

            using ContextBase_ptr = std::shared_ptr<ContextBase>;
        } // namespace API
} // namespace Epsilon