#pragma once

#include <string>
#if BUILD_ANDROID
#undef USE_GLFW
#else

#if BUILD_ANDROID == 0
#include <vulkan/vulkan.hpp>
#endif
#endif

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#if USE_GLFW
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#endif
#ifdef __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#if defined(ANDROID) || defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#undef USE_GLFW
#include <jni.h>
#include <android/native_window.h>
#endif

#if USE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace framework
{
    class Window
    {

    public:
#if USE_GLFW
        using windowType = GLFWwindow;
#elif _WIN32
        using windowType = HWND;
#elif defined(ANDROID) || defined(__ANDROID__)
        using windowType = ANativeWindow;
#endif
    public:
        void init(std::string appName, int w = 1280, int h = 720);

        void mainLoop()
        {
#if USE_GLFW
            while (!glfwWindowShouldClose(mWindow))
            {
                glfwPollEvents();
            }
#else
#ifdef _WIN32

#endif
#endif
        }

        bool ShouldClose()
        {
#if USE_GLFW
            return glfwWindowShouldClose(mWindow);
#endif
        }

        void PollEvents()
        {
#if USE_GLFW
            glfwPollEvents();
#else
#ifdef _WIN32
            MSG message;

            while (GetMessage(&message, NULL, 0, 0))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
#endif
#endif
        }

        void ShowCursor()
        {
#if USE_GLFW
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
        }

        void HideCursor()
        {
#if USE_GLFW
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
        }

        void setCursorPosition(int x, int y)
        {
#if USE_GLFW
            glfwSetCursorPos(mWindow, x, y);
#endif
        }

        std::pair<int, int> getSize()
        {
            /*#if USE_GLFW
                        int w, h;
                        glfwGetWindowSize(mWindow, &w, &h);*/
            return {mWidth, mHeight}; /*
 #endif*/
        }

        void cleanup()
        {
#if USE_GLFW
            glfwDestroyWindow(mWindow);
            glfwTerminate();
#endif
        }

        void setWindowTitle(const char *title)
        {
#if USE_GLFW
            glfwSetWindowTitle(mWindow, title);
#endif
        }

        windowType *getWindow() const
        {
            return mWindow;
        }

        void setWindow(windowType *window)
        {
            mWindow = window;
        }

    private:
        unsigned mWidth = 0;
        unsigned mHeight = 0;

        windowType *mWindow;
#if defined(_WIN32) && (USE_GLFW == false)
        ATOM WINAPI RegisterClassEx(
            const WNDCLASSEX *lpwcx);
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_DESTROY:
                DestroyWindow(hWnd);
                PostQuitMessage(0);
                break;
            case WM_PAINT:
                ValidateRect(hWnd, NULL);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
                break;
            }
        }
        HINSTANCE windowInstance;
#endif
    };
}