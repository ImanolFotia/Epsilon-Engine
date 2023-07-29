///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

#if USE_GLFW
#include "GLFW/glfw3.h"
#endif

#include "KeyboardWrapper.hpp"

#include <beacon/beacon.hpp>


#if defined(_WIN32)
//  Microsoft
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __declspec(dllexport)
#else
#define EPSILON_DLL __declspec(dllimport)
#endif
#elif defined(__linux__)
//  GCC
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __attribute__((visibility("default")))
#else
#define EPSILON_DLL
#endif
#else
//  do nothing and hope for the best?
#define EPSILON_DLL
#pragma warning Unknown dynamic link import / export semantics.
#endif

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

        class EPSILON_DLL KeyBoard
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
