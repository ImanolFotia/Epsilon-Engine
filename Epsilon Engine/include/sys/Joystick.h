#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

namespace Input
{
    class Joystick
    {
    public:
        static bool JoystickIsPresent;

        static int JoystickAxesCount;
        static int JoystickButtonsCount;
        static const float* JoystickAxes;
        static const unsigned char* JoystickButtons;

        static void JoystickCallback(int joystick, int event)
        {
            if (event == GLFW_CONNECTED)
            {
                std::cout << "New joystick connected: " << glfwGetJoystickName(joystick) << std::endl;
                JoystickIsPresent = true;

               JoystickAxes = glfwGetJoystickAxes(joystick, &JoystickAxesCount);
               std::cout << "Axis Count: " << JoystickAxesCount << std::endl;


               JoystickButtons = glfwGetJoystickButtons(joystick, &JoystickButtonsCount);
               std::cout << "Axis Count: " << JoystickButtonsCount << std::endl;
            }
            else if (event == GLFW_DISCONNECTED)
            {
                JoystickIsPresent = false;
            }

        }

        static void PollJoystick()
        {

            if(JoystickIsPresent)
            {

               JoystickAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &JoystickAxesCount);

               JoystickButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &JoystickButtonsCount);
            }
        }
    };
}
