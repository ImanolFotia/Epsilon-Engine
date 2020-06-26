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

            void Context::Init(std::shared_ptr<Platform::WindowHandle<>> windowHandle, CONTEXT_TYPE type)
            {
                mWindowHandle = windowHandle;
                mType = type;

#ifndef EPSILON_STATIC
                glfwMakeContextCurrent(mWindowHandle->getHandle());

                glewExperimental = GL_TRUE;
                glewInit();
#else
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
            }
        } // namespace OpenGL
    }     // namespace API
} // namespace Epsilon