#pragma once

#include "Driver/API/Context.hpp"
#include "Log.h"


namespace Epsilon
{
    namespace Platform
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

                    void Init(std::shared_ptr<WindowHandle<>>, CONTEXT_TYPE) override;
                    void SwapBuffers() override;
                    void Shutdown() override;
                };
            } // namespace OpenGL
        } // namespace API
    } // namespace Platform
} // namespace Epsilon