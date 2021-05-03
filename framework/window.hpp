#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

        void cleanup()
        {
            glfwDestroyWindow(mWindow);
            glfwTerminate();
        }

    private:
        unsigned mWidth = 1280;
        unsigned mHeight = 720;

        GLFWwindow *mWindow;
    };
}