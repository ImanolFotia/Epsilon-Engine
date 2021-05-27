#pragma once

#include "Driver/API/Context.hpp"
#include "Log.h"

namespace Epsilon
{
    class WindowBase;

    namespace API::OpenGL
    {
        struct oglContextInfo_t {
            const char* vendor;
            const char* version;
            const char* device_name;
        };
        
        class Context : public ContextBase
        {
        public:
            Context();

            void Init(CONTEXT_TYPE) override;
            void AttachContext(std::shared_ptr<Platform::WindowHandle<>> windowHandle) override;
            void SwapBuffers() override;
            void Shutdown() override;

        private:
            void getConextInfo();
        };
    } // namespace OpenGL
} // namespace Epsilon