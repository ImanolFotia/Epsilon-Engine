#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#include <beacon/beacon.hpp>

namespace framework
{
    namespace Input
    {

        enum STATE
        {
            PRESSED,
            RELEASED,
            UP,
            DOWN,
            NONE
        };

        struct Button
        {
            STATE State = NONE;

            double xOffset = 0.0;
            double yOffset = 0.0;
            
        };

        class MouseArgs : public beacon::args
        {
        public:
            const Button &Right() { return right; }
            const Button &Middle() { return middle; }
            const Button &Left() { return left; }

            const Button &Wheel() { return wheel; }

            const double &X() { return x; }
            const double &Y() { return y; }

        private:
            Button right;
            Button middle;
            Button left;
            Button wheel;

            double x = 0.0;
            double y = 0.0;

            friend class Mouse;
        };

        class Mouse
        {

            //static MouseArgs mouseArgs;

        public:
            static double XPOS;
            static double YPOS;

            static STATE RIGHT;
            static STATE MIDDLE;
            static STATE LEFT;

            static beacon::single_handler<MouseArgs> MouseEventHandler;

            static double prevxOffset;
            static double prevyOffset;

            static void MouseCallBackGLFW(GLFWwindow *window, double xpos, double ypos)
            {
                MouseArgs mouseArgs;
                XPOS = xpos;
                YPOS = ypos;

                mouseArgs.x = xpos;
                mouseArgs.y = ypos;

                MouseEventHandler.raise(nullptr, &mouseArgs);
            }

            static void MouseWheelCallbackGLFW(GLFWwindow *window, double xoffset, double yoffset)
            {
                MouseArgs mouseArgs;
                if (yoffset >= 0)
                    mouseArgs.wheel.State = UP;
                else
                    mouseArgs.wheel.State = DOWN;

                mouseArgs.wheel.xOffset = xoffset;
                mouseArgs.wheel.yOffset = yoffset;

                
                mouseArgs.x = XPOS;
                mouseArgs.y = YPOS;

                MouseEventHandler.raise(nullptr, &mouseArgs);
                prevyOffset = yoffset;
            }

            static void MouseButtonCallbackGLFW(GLFWwindow *window, int button, int action, int mods)
            {
                MouseArgs mouseArgs;
                if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
                {
                    RIGHT = PRESSED;
                }
                if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
                {
                    RIGHT = RELEASED;
                }
                if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
                {
                    MIDDLE = PRESSED;
                }
                if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
                {
                    MIDDLE = RELEASED;
                }
                if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
                {
                    LEFT = PRESSED;
                }
                if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
                {
                    LEFT = RELEASED;
                }

                mouseArgs.right.State = RIGHT;
                mouseArgs.left.State = LEFT;
                mouseArgs.middle.State = MIDDLE;

                
                mouseArgs.x = XPOS;
                mouseArgs.y = YPOS;

                MouseEventHandler.raise(nullptr, &mouseArgs);
            }
        };
    }
}
