#include <Core.hpp>
#include <IO/IO.hpp>

#include "Context.hpp"

#include <GLFW/glfw3.h>
#ifndef EPSILON_STATIC
#include <GLFW/glfw3.h>
#endif

namespace Epsilon::API::OpenGL
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
        /*#ifndef EPSILON_STATIC
                glewExperimental = GL_TRUE;
                glewInit();
#else*/
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            IO::PrintLine("Failed to initialize GLAD");
            std::exit(255);
            return;
        }
        /*#endif*/
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
} // namespace Epsilon
