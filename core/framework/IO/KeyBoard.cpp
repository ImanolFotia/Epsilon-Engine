
#include "KeyBoard.hpp"

namespace framework
{

    //std::unordered_map<const char *, uint16_t> Input::KeyWraps::kw;

    bool Input::KeyBoard::KEYS[1024] = { false };

    //std::unordered_map<const char *, uint16_t> Input::KeyWraps::kw;

    Input::KeyWraps Input::KeyBoard::KeyWrap;
    namespace Input
    {


#if USE_GLFW
        void KeyBoard::KeyBoardCallBackGLFW(GLFWwindow *window, int key, int scancode, int action, int mode)
        {
            if (key >= 0 && key < 1024)
            {
                KeyboardArgs args;
                if (action == GLFW_PRESS)
                {
                    KEYS[key] = true;
                    args.key_down_index = key;
                }
                else if (action == GLFW_RELEASE)
                {
                    KEYS[key] = false;
                    args.key_up_index = key;
                }

                KeyboardEventHandler.raise(nullptr, &args);
            }
        }
#endif
        bool KeyBoard::getKey(uint16_t key)
        {
            return KEYS[key];
        }

        bool KeyBoard::getKey(const char *alias)
        {
            return KEYS[KeyWrap.getKey(alias)];
        }

        void KeyBoard::wrapKey(const char *alias, uint16_t key)
        {
            KeyWrap.addKey(alias, key);
        }

    }
}