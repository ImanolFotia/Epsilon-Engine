#include <sys/KeyBoard.h>
#include <iostream>

bool Input::KeyBoard::KEYS[1024] = {false};

namespace Input
{

void KeyBoard::KeyBoardCallBack(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            KEYS[key] = true;
            //std::cout << "Pressed: " << KEYS[key] << std::endl;
        }
        else if (action == GLFW_RELEASE)
        {
            KEYS[key] = false;
            //std::cout << "Released: " << KEYS[key] << std::endl;
        }

    }
}
}
