#include <Core.hpp>
#include <IO/IO.hpp>

#include "Context.hpp"

#ifndef EPSILON_STATIC
#include <GLFW/glfw3.h>
#endif

namespace Epsilon
{
    namespace API
    {
        namespace OpenGL
        {
            Context::Context()
            {
            }

            void Context::Init(CONTEXT_TYPE type)
            {
                mType = type;
            }

            void Context::AttachContext(std::shared_ptr<Platform::WindowHandle<>> windowHandle)
            {

                mWindowHandle = windowHandle;
#ifndef EPSILON_STATIC
                glewExperimental = GL_TRUE;
                glewInit();
#else
                if (mWindowHandle->getType() == Platform::WINDOW_HANDLE_TYPE::GLAD)
                {
                    if (!gladLoadGL())
                    {
                        IO::PrintLine("Failed to initialize GLAD");
                        return;
                    }
#endif
            }

            void Context::SwapBuffers()
            {
#ifndef EPSILON_STATIC
                if (mWindowHandle->getType() == Platform::WINDOW_HANDLE_TYPE::GLFW)
                    glfwSwapBuffers(mWindowHandle->getHandle());
#else
                    if (mWindowHandle->getType() == Platform::WINDOW_HANDLE_TYPE::HDC)
                        ::SwapBuffers((HDC)mWindowHandle->getHandle()); //Calling SwapBuffers inside windows.h

#endif
            }

            void Context::Shutdown()
            {
#ifndef EPSILON_STATIC
                if (mWindowHandle->getType() == Platform::WINDOW_HANDLE_TYPE::GLFW)
                    glfwTerminate();
#endif
            }
        } // namespace OpenGL
    }     // namespace API
} // namespace Epsilon