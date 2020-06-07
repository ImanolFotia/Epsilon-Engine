
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#endif

#include "Context.hpp"
namespace Epsilon
{
    namespace Platform
    {
        namespace API
        {
            namespace OpenGL
            {
                Context::Context()
                {
                }

                void Context::Init(std::shared_ptr<WindowHandle<>> windowHandle, CONTEXT_TYPE type)
                {
                    mWindowHandle = windowHandle;
                    mType = type;

                    glfwMakeContextCurrent(mWindowHandle->getHandle());

                    glewExperimental = GL_TRUE;
                    glewInit();
                }

                void Context::SwapBuffers()
                {
                    glfwSwapBuffers(mWindowHandle->getHandle());
                }

                void Context::Shutdown()
                {
                }
            } // namespace OpenGL
        }     // namespace API
    }         // namespace Platform
} // namespace Epsilon