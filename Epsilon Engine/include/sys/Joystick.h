#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

namespace Input
{
    class Joystick
    {
    public:
        static int JoistickIsPresent;

        static void JoystickCallback(int joystick, int event)
        {
            if (event == GLFW_CONNECTED)
            {
                std::cout << "New joystick connected: " << glfwGetJoystickName(GLFW_JOYSTICK_1) << std::endl;
            }
            else if (event == GLFW_DISCONNECTED)
            {

            }
        }
    };
}
