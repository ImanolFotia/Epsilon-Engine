#pragma once

#include "Driver/API/Context.hpp"
#include "Log.h"

namespace Epsilon
{
    class WindowBase;

    namespace API::OpenGL
    {
        class Context : public ContextBase
        {
        public:
            Context();

            void Init(CONTEXT_TYPE) override;
            void AttachContext(std::shared_ptr<Platform::WindowHandle<>> windowHandle) override;
            void SwapBuffers() override;
            void Shutdown() override;
        };
    } // namespace OpenGL
} // namespace Epsilon