#pragma once

#include <pch.hpp>
#include <Core.hpp>

#include "Platform/WindowHandle.hpp"

namespace Epsilon::API
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
        ContextBase() {}
        virtual ~ContextBase() {}
        virtual void Init(CONTEXT_TYPE) = 0;
        virtual void AttachContext(std::shared_ptr<Platform::WindowHandle<>>) = 0;
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
} // namespace Epsilon