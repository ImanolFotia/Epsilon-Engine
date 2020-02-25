#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <memory>

#include <sys/JoystickWrapper.h>

namespace Input
{

static const int MAX_JOYSTICKS = 4;

class Joystick
{
public:
    bool JoystickIsPresent = false;
    bool JoystickWasPresent = false;
    int JoystickAxesCount = 0;
    int JoystickButtonsCount = 0;
    int JoystickId = -1;
    const char *Name = nullptr;
    const float *JoystickAxes = nullptr;
    const unsigned char *JoystickButtons = nullptr;

    bool BUTTONS[15] = {false};

    Joystick()
    {

        JoystickIsPresent = false;
        JoystickWasPresent = false;
        JoystickAxesCount = 0;
        JoystickButtonsCount = 0;
        JoystickId = -1;
        Name = nullptr;
        JoystickAxes = nullptr;
        JoystickButtons = nullptr;

        BUTTONS[15] = {false};
    }

    glm::vec2 L3() const
    {
        if (JoystickAxes != nullptr)
            return glm::vec2(JoystickAxes[0], JoystickAxes[1]);
        return glm::vec2(0.0f);
    }

    glm::vec2 R3() const
    {
        if (JoystickAxes != nullptr)
            return glm::vec2(JoystickAxes[2], JoystickAxes[3]);
        return glm::vec2(0.0f);
    }
};

class JoystickManager
{
    using Joystick_ptr = std::shared_ptr<Joystick>;

public:
    JoystickManager()
    {
    }

    static Joystick_ptr dummyJoystick;

    /*
        Returns a pointer the primary joystick
        If no joystick is connected, it returns a pointer to an empty Joystick object with 
        the Joystick::JoystickIsPresent flag disabled
    */
    static const Joystick_ptr PrimaryJoystick()
    {
        if (JoystickVector.size() > 0)
        {
            if (JoystickVector.at(0) != nullptr)
            {
                if (JoystickVector.at(0)->JoystickIsPresent)
                {
                    return JoystickVector.at(0);
                }
            }
        }

        return dummyJoystick;
    }

    static void JoystickCallback(int joystick, int event)
    {
        try
        {
            if (event == GLFW_CONNECTED)
            {
                Joystick_ptr _joystick = std::make_shared<Joystick>();
                std::cout << "New joystick connected: " << glfwGetJoystickName(joystick) << std::endl;
                _joystick->JoystickIsPresent = true;
                _joystick->JoystickWasPresent = true;
                _joystick->JoystickId = joystick;
                _joystick->Name = glfwGetJoystickName(joystick);
                _joystick->JoystickAxes = glfwGetJoystickAxes(joystick, &_joystick->JoystickAxesCount);
                std::cout << "Axis Count: " << _joystick->JoystickAxesCount << std::endl;

                _joystick->JoystickButtons = glfwGetJoystickButtons(joystick, &_joystick->JoystickButtonsCount);
                std::cout << "Axis Count: " << _joystick->JoystickButtonsCount << std::endl;

                JoystickVector[joystick] = _joystick;
            }
            else if (event == GLFW_DISCONNECTED)
            {
                JoystickVector[joystick]->JoystickIsPresent = false;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << "\n line: " << __LINE__ << ".\n"
                      << "What(): " << e.what() << std::endl;
        }
    }

    static void DetectJoysticks()
    {
        try
        {
            for (int i = 0; i < MAX_JOYSTICKS; ++i)
            {
                if (glfwJoystickPresent(i))
                {
                    if (JoystickVector.at(i) == nullptr)
                    {
                        Joystick_ptr _joystick = std::make_shared<Joystick>();
                        std::cout << "New joystick connected: " << glfwGetJoystickName(i) << std::endl;
                        _joystick->JoystickIsPresent = true;
                        _joystick->JoystickWasPresent = true;
                        _joystick->JoystickId = i;
                        _joystick->Name = glfwGetJoystickName(i);
                        _joystick->JoystickAxes = glfwGetJoystickAxes(i, &_joystick->JoystickAxesCount);
                        std::cout << "Axis Count: " << _joystick->JoystickAxesCount << std::endl;

                        _joystick->JoystickButtons = glfwGetJoystickButtons(i, &_joystick->JoystickButtonsCount);
                        std::cout << "Axis Count: " << _joystick->JoystickButtonsCount << std::endl;

                        JoystickVector[i] = _joystick;
                    }
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << "\n line: " << __LINE__ << ".\n"
                      << "What(): " << e.what() << std::endl;
        }
    }

    static void PollJoystick()
    {
        try
        {
            for (int i = 0; i < JoystickVector.size(); ++i)
            {
                if (JoystickVector.at(i) != nullptr)
                    if (JoystickVector.at(i)->JoystickIsPresent)
                    {
                        Joystick_ptr _joystick = JoystickVector.at(i);
                        _joystick->JoystickAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &_joystick->JoystickAxesCount);

                        _joystick->JoystickButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &_joystick->JoystickButtonsCount);

                        for (int i = 0; i < 15; i++)
                        {
                            if (_joystick->JoystickButtons[i] == GLFW_PRESS)
                            {
                                _joystick->BUTTONS[i] = true;
                            }
                            else if (_joystick->JoystickButtons[i] == GLFW_RELEASE)
                            {
                                _joystick->BUTTONS[i] = false;
                            }
                        }
                    }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << "\n line: " << __LINE__ << ".\n"
                      << "What(): " << e.what() << std::endl;
        }
    }

public:
    static std::vector<Joystick_ptr> JoystickVector;
};

} // namespace Input
