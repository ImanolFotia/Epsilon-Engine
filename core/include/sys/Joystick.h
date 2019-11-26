#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

#include <sys/JoystickWrapper.h>

namespace Input {
class Joystick {
public:
    static bool JoystickIsPresent;

    static int JoystickAxesCount;
    static int JoystickButtonsCount;
    static const float* JoystickAxes;
    static const unsigned char* JoystickButtons;

    static bool BUTTONS[15];

    static void JoystickCallback(int joystick, int event) {
        if (event == GLFW_CONNECTED) {
            std::cout << "New joystick connected: " << glfwGetJoystickName(joystick) << std::endl;
            JoystickIsPresent = true;

            JoystickAxes = glfwGetJoystickAxes(joystick, &JoystickAxesCount);
            std::cout << "Axis Count: " << JoystickAxesCount << std::endl;


            JoystickButtons = glfwGetJoystickButtons(joystick, &JoystickButtonsCount);
            std::cout << "Axis Count: " << JoystickButtonsCount << std::endl;

        } else if (event == GLFW_DISCONNECTED) {
            JoystickIsPresent = false;
        }

    }

    static void PollJoystick() {

        if(JoystickIsPresent) {

            JoystickAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &JoystickAxesCount);

            JoystickButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &JoystickButtonsCount);

            for(int i = 0; i < 15; i++){
                if (JoystickButtons[i] == GLFW_PRESS) {
                    BUTTONS[i] = true;
                    //std::cout << "Pressed: " << KEYS[key] << std::endl;
                } else if (JoystickButtons[i] == GLFW_RELEASE) {
                    BUTTONS[i] = false;
                    //std::cout << "Released: " << KEYS[key] << std::endl;
                }

            }

        } else {
            JoystickButtons = nullptr;
        }
    }
};
}
