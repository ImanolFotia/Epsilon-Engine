///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

#include <GLFW/glfw3.h>
#include <IO/KeyboardWrapper.h>
#include <unordered_map>

namespace Input
{

    class KeyWraps
    {
    public:
        void addKey(const char *alias, uint16_t key)
        {
            kw[alias] = key;
        }

        uint16_t getKey(const char *alias)
        {
            return kw.at(alias);
        }
        private:
        std::unordered_map<const char *, uint16_t> kw;
    };

    class KeyBoard
    {
    private:

    public:
        static KeyWraps KeyWrap;

        static bool KEYS[1024];

        static void KeyBoardCallBack(GLFWwindow *window, int key, int scancode, int action, int mode);

        static bool getKey(uint16_t);

        static bool getKey(const char *);

        static void wrapKey(const char *, uint16_t);
    };

} // namespace Input
