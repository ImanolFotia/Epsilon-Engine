///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

#if USE_GLFW
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#endif

#include "KeyboardWrapper.hpp"

#include <beacon/beacon.hpp>

namespace framework
{
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

        class KeyboardArgs : public beacon::args {
            public:
                int key_up_index = -1;
                int key_down_index = -1;
        };

        class KeyBoard
        {
        private:
        public:
            static KeyWraps KeyWrap;

            static bool KEYS[1024];

#if USE_GLFW
            static void KeyBoardCallBackGLFW(GLFWwindow *window, int key, int scancode, int action, int mode);
#endif
            static bool getKey(uint16_t);

            static bool getKey(const char *);

            static void wrapKey(const char *, uint16_t);
            
            static beacon::single_handler<KeyboardArgs> KeyboardEventHandler;
        };

    } // namespace Input
}
