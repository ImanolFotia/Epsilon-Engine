#include "Window.hpp"

#ifdef WIN32
namespace Epsilon
{
    namespace Platform
    {
        namespace Windows
        {
            Window::Window(API::CONTEXT_TYPE type)
            {
                mContextType = type;
                using API::CONTEXT_TYPE;
                switch (type)
                {
                case CONTEXT_TYPE::OGL:
                    mContext = std::make_shared<API::OpenGL::Context>();
                    mContext->Init(API::CONTEXT_TYPE::OGL);
                    break;
                case CONTEXT_TYPE::VULKAN:
                    mContext = std::make_shared<API::Vulkan::Context>();
                    mContext->Init(API::CONTEXT_TYPE::VULKAN);
                    break;
                default:
                    std::cout << "This API is currently not supported" << std::endl;
                }
            }

            void Window::Init(const char *title, int w, int h)
            {

                if (!glfwInit())
                {
                    Log::WriteToLog("GLFW could not be initialized");
                }

                int lNumberOfMonitors = 0;

                GLFWmonitor **lMonitor = glfwGetMonitors(&lNumberOfMonitors);

                for (int i = 0; i < lNumberOfMonitors; ++i)
                {
                    int numberofmodes;
                    const GLFWvidmode *modes = glfwGetVideoModes(lMonitor[i], &numberofmodes);
                    int widthMM, heightMM;
                    glfwGetMonitorPhysicalSize(lMonitor[i], &widthMM, &heightMM);
                    const GLFWvidmode *mode = glfwGetVideoMode(lMonitor[i]);
                    const double dpi = mode->width / (widthMM / 25.4);
                }

                if (mContext->getType() == API::CONTEXT_TYPE::OGL)
                {
                    setupOpenGL();
                }

                if (mContext->getType() == API::CONTEXT_TYPE::VULKAN)
                {
                    setupVulkan();
                }

                glfwSetErrorCallback([](int error, const char *description) {
                    fprintf(stderr, "Error: %s\n", description);
                });

                glfwSetWindowSizeCallback(mWindowHandle->getHandle(), [](GLFWwindow *window, int w, int h) {
                    mWindowData.Width = w;
                    mWindowData.Height = h;
                    Window::mNeedsToResize = true;
                });

                //Set up IO callbacks
                glfwSetKeyCallback(mWindowHandle->getHandle(), Input::KeyBoard::KeyBoardCallBackGLFW);
                glfwSetCursorPosCallback(mWindowHandle->getHandle(), Input::Mouse::MouseCallBackGLFW);
                glfwSetJoystickCallback(Input::Joystick::JoystickManager::JoystickCallbackGLFW);
                glfwSetMouseButtonCallback(mWindowHandle->getHandle(), Input::Mouse::MouseButtonCallbackGLFW);
                glfwSetScrollCallback(mWindowHandle->getHandle(), Input::Mouse::MouseWheelCallbackGLFW);

                glfwSetInputMode(mWindowHandle->getHandle(), GLFW_STICKY_KEYS, true);
                glfwSetInputMode(mWindowHandle->getHandle(), GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
            }
            void Window::Destroy()
            {
                glfwDestroyWindow(mWindowHandle->getHandle());
                glfwTerminate();
            }
            void Window::SwapBuffers()
            {
                mContext->SwapBuffers();
                mNeedsToResize = false;
            }

            void Window::ShowCursor()
            {
                glfwSetInputMode(mWindowHandle->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            void Window::HideCursor()
            {
                glfwSetInputMode(mWindowHandle->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            bool Window::WantsToClose()
            {
                return glfwWindowShouldClose(mWindowHandle->getHandle());
            }

            void Window::OnResize(GLFWwindow *window, int w, int h)
            {
                mWindowData.Width = w;
                mWindowData.Height = h;
            }

            bool Window::NeedsToResize()
            {
                return mNeedsToResize;
            }

            unsigned Window::FrameNumber()
            {
                return mFrameNumber;
            }

            void Window::setupOpenGL()
            {
                
                int lNumberOfMonitors = 0;

                GLFWmonitor **lMonitor = glfwGetMonitors(&lNumberOfMonitors);

                GLFWmonitor *lCurrentMonitor;

                if (mWindowData.CurrentMonitor > lNumberOfMonitors)
                    lCurrentMonitor = glfwGetPrimaryMonitor();
                else
                    lCurrentMonitor = lMonitor[mWindowData.CurrentMonitor];

                int numberofmodes;
                const GLFWvidmode *modes = glfwGetVideoModes(lCurrentMonitor, &numberofmodes);

                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_MAXIMIZED , GL_TRUE);

                const GLFWvidmode *mode = glfwGetVideoMode(lCurrentMonitor);
                glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, 60);
                glfwWindowHint(GLFW_RED_BITS, 16);
                glfwWindowHint(GLFW_GREEN_BITS, 16);
                glfwWindowHint(GLFW_BLUE_BITS, 16);

                //Window is created in full screen mode

                GLFWwindow *window; // = mWindowHandle->getHandle();
                mWindowHandle = std::make_shared<Platform::WindowHandle<GLFWwindow>>(Platform::WINDOW_HANDLE_TYPE::GLFW);
                if (mWindowData.State == 1)
                {
                    window = glfwCreateWindow(mWindowData.Width, mWindowData.Height, mWindowData.Title, lCurrentMonitor, 0);
                }
                //Window is created in borderless fullscreen mode
                else if (mWindowData.State == 2 && mode->width == mWindowData.Width && mode->height == mWindowData.Height)
                {
                    mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
                    window = glfwCreateWindow(mode->width, mode->height, mWindowData.Title, lCurrentMonitor, 0);
                }
                else if (mWindowData.State == 2 && mode->width != mWindowData.Width && mode->height != mWindowData.Height)
                {
                    mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
                    window = glfwCreateWindow(mWindowData.Width, mWindowData.Height, mWindowData.Title, nullptr, 0);
                    glfwSetWindowPos(window, (mode->width / 2) - (mWindowData.Width * 0.5), (mode->height / 2) - (mWindowData.Height * 0.5));
                }
                //Window is created in windowed mode and centered in the middle of the display
                else
                {
                    window = glfwCreateWindow(mWindowData.Width, mWindowData.Height, mWindowData.Title, nullptr, nullptr);
                    glfwSetWindowPos(window, (mode->width / 2) - (mWindowData.Width * 0.5), (mode->height / 2) - (mWindowData.Height * 0.5));
                }

                mWindowHandle->SetHandle(window);

                if (window == nullptr)
                    std::cout << "window is null" << std::endl;
                glfwMakeContextCurrent(window);

                mContext->AttachContext(mWindowHandle);
                
                glfwSwapInterval(mWindowData.VSync);
            }

            void Window::setupVulkan() {
                
                int lNumberOfMonitors = 0;

                GLFWmonitor **lMonitor = glfwGetMonitors(&lNumberOfMonitors);

                GLFWmonitor *lCurrentMonitor;

                if (mWindowData.CurrentMonitor > lNumberOfMonitors)
                    lCurrentMonitor = glfwGetPrimaryMonitor();
                else
                    lCurrentMonitor = lMonitor[mWindowData.CurrentMonitor];

                int numberofmodes;
                const GLFWvidmode *modes = glfwGetVideoModes(lCurrentMonitor, &numberofmodes);

                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

                const GLFWvidmode *mode = glfwGetVideoMode(lCurrentMonitor);
                glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, 60);
                glfwWindowHint(GLFW_RED_BITS, 16);
                glfwWindowHint(GLFW_GREEN_BITS, 16);
                glfwWindowHint(GLFW_BLUE_BITS, 16);

                //Window is created in full screen mode

                GLFWwindow *window; // = mWindowHandle->getHandle();
                mWindowHandle = std::make_shared<Platform::WindowHandle<GLFWwindow>>(Platform::WINDOW_HANDLE_TYPE::GLFW);
                if (mWindowData.State == 1)
                {
                    window = glfwCreateWindow(mWindowData.Width, mWindowData.Height, mWindowData.Title, lCurrentMonitor, 0);
                }
                //Window is created in borderless fullscreen mode
                else if (mWindowData.State == 2 && mode->width == mWindowData.Width && mode->height == mWindowData.Height)
                {
                    mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
                    window = glfwCreateWindow(mode->width, mode->height, mWindowData.Title, lCurrentMonitor, 0);
                }
                else if (mWindowData.State == 2 && mode->width != mWindowData.Width && mode->height != mWindowData.Height)
                {
                    mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
                    window = glfwCreateWindow(mWindowData.Width, mWindowData.Height, mWindowData.Title, nullptr, 0);
                    glfwSetWindowPos(window, (mode->width / 2) - (mWindowData.Width * 0.5), (mode->height / 2) - (mWindowData.Height * 0.5));
                }
                //Window is created in windowed mode and centered in the middle of the display
                else
                {
                    window = glfwCreateWindow(mWindowData.Width, mWindowData.Height, mWindowData.Title, nullptr, nullptr);
                    glfwSetWindowPos(window, (mode->width / 2) - (mWindowData.Width * 0.5), (mode->height / 2) - (mWindowData.Height * 0.5));
                }

                mWindowHandle->SetHandle(window);

                if (window == nullptr)
                    std::cout << "window is null" << std::endl;
                glfwMakeContextCurrent(window);

                mContext->AttachContext(mWindowHandle);
            }
        } // namespace Windows
    }     // namespace Platform
} // namespace Epsilon

#endif