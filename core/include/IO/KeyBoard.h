///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

#include <GLFW/glfw3.h>
#include <IO/KeyboardWrapper.h>

namespace Input
{

    class KeyBoard
    {

    public:
        static bool KEYS[1024];

        static void KeyBoardCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);

    };

}

