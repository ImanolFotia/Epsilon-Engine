#pragma once

#include <memory>
#include <iostream>

struct GLFWwindow;

namespace Epsilon
{
    namespace Platform
    {

        enum WINDOW_HANDLE_TYPE
        {
            HDC = 0,
            GLFW,
            GLUT,
            SFML,
            X11
        };
        template <typename T = GLFWwindow>
        class WindowHandle
        {

        public:
            WindowHandle(WINDOW_HANDLE_TYPE type) : mType(type)
            {
            }

            void SetHandle(T* handle)
            {
                mHandle = handle;
            }

            T* getHandle()
            {
                return mHandle;
            }

            const WINDOW_HANDLE_TYPE getType()
            {
                return mType;
            }

            void operator=(T* t)
            {
                std::cout << "window assigned" << std::endl;
                mHandle = t;
            }

        private:
            T* mHandle;
            WINDOW_HANDLE_TYPE mType;
        };

    } // namespace Platform
} // namespace Epsilon