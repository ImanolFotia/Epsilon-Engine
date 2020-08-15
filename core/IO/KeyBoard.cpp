
#include <pch.hpp>
#include <IO/KeyBoard.h>

bool Input::KeyBoard::KEYS[1024] = {false};

//std::unordered_map<const char *, uint16_t> Input::KeyWraps::kw;

Input::KeyWraps Input::KeyBoard::KeyWrap;


namespace Input
{

void KeyBoard::KeyBoardCallBackGLFW(GLFWwindow* window, int key, int scancode, int action, int mode)
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

bool KeyBoard::getKey(uint16_t key) {
    return KEYS[key];
}

bool KeyBoard::getKey(const char* alias) {
    return KEYS[KeyWrap.getKey(alias)];
}

void KeyBoard::wrapKey(const char* alias, uint16_t key){
    KeyWrap.addKey(alias, key);
}

}
