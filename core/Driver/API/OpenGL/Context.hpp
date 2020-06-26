#pragma once

#include "Driver/API/Context.hpp"
#include "Log.h"

namespace Epsilon
{
    class WindowBase;

    namespace API
    {
        namespace OpenGL
        {
            class Context : public ContextBase
            {
            public:
                Context();

                void Init(std::shared_ptr<Platform::WindowHandle<>>, CONTEXT_TYPE) override;
                void SwapBuffers() override;
                void Shutdown() override;
            };
        } // namespace OpenGL
    }     // namespace API
} // namespace Epsilon