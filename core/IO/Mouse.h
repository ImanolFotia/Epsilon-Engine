#pragma once

#include <iostream>

namespace Input
{
    class Mouse
    {
    public:

        enum STATE {
            PRESSED,
            RELEASED
        };

        static double XPOS;
        static double YPOS;

        static STATE RIGHT;
        static STATE MIDDLE;
        static STATE LEFT;

        static void MouseCallBackGLFW(GLFWwindow* window, double xpos, double ypos)
        {
            XPOS = xpos;
            YPOS = ypos;
        }

        static void MouseButtonCallbackGLFW(GLFWwindow* window, int button, int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                RIGHT = PRESSED;
            }
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
                RIGHT = RELEASED;
            }
            if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
                MIDDLE = PRESSED;
            }
            if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
                MIDDLE = RELEASED;
            }
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                LEFT = PRESSED;
            }
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
                LEFT = RELEASED;
            }
        }
    };

}
