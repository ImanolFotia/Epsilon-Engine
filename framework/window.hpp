#pragma once

#include <string>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace LearningVulkan
{
    class Window
    {
    public:
        void init(std::string appName, int w = 1280, int h = 720)
        {
            mWidth = w;
            mHeight = h;

            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            mWindow = glfwCreateWindow(mWidth, mHeight, appName.c_str(), nullptr, nullptr);
        }

        void mainLoop()
        {
            while (!glfwWindowShouldClose(mWindow))
            {
                glfwPollEvents();
            }
        }

        bool ShouldClose() {
            return glfwWindowShouldClose(mWindow);
        }

        void PollEvents() {
            glfwPollEvents();
        }

        void cleanup()
        {
            glfwDestroyWindow(mWindow);
            glfwTerminate();
        }

        GLFWwindow* getWindow() {
            return mWindow;
        }

    private:
        unsigned mWidth = 1280;
        unsigned mHeight = 720;

        GLFWwindow *mWindow;
    };
}