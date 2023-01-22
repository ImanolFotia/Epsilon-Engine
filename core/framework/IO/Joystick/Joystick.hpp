#pragma once
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

#include "JoystickWrapper.hpp"
#include "Mappings.hpp"
namespace framework
{
    namespace Input
    {

        namespace Joystick
        {

            static const int MAX_JOYSTICKS = 4;

            class Joystick
            {
                using Mapping_ptr = std::shared_ptr<Input::Joystick::Mappings::Mapping>;

            public:
                Joystick()
                {

                    mJoystickIsPresent = false;
                    mJoystickWasPresent = false;
                    mJoystickAxesCount = 0;
                    mJoystickButtonsCount = 0;
                    mJoystickId = -1;
                    mName = nullptr;
                    mJoystickAxes = nullptr;
                    mJoystickButtons = nullptr;

                    mMapping = std::make_shared<Input::Joystick::Mappings::Generic>();
                }

                glm::vec2 L3() const
                {
                    if (mJoystickAxes != nullptr)
                        return glm::vec2(mJoystickAxes[mMapping->DPAD_LEFT_X()], mJoystickAxes[mMapping->DPAD_LEFT_Y()]);
                    return glm::vec2(0.0f);
                }

                glm::vec2 R3() const
                {
                    if (mJoystickAxes != nullptr)
                        return glm::vec2(mJoystickAxes[mMapping->DPAD_RIGHT_X()], mJoystickAxes[mMapping->DPAD_RIGHT_Y()]);
                    return glm::vec2(0.0f);
                }

                //Check is button is pressed
                //Returns false (not pressed) if button is out of range
                bool isButtonPressed(int button)
                {
                    if (button < mJoystickButtonsCount)
                        return mButtons.at(button);
                    return false;
                }

                bool getJoystickIsPresent() { return mJoystickIsPresent; }
                bool getJoystickWasPresent() { return mJoystickWasPresent; }
                unsigned getJoystickAxesCount() { return mJoystickAxesCount; }
                unsigned getJoystickButtonsCount() { return mJoystickButtonsCount; }
                unsigned getJoystickId() { return mJoystickId; }
                const char *getName() { return mName; }
                const float *getJoystickAxes() { return mJoystickAxes; }
                const unsigned char *getJoystickButtons() { return mJoystickButtons; }

                const unsigned char getJoystickButton(unsigned i)
                {
                    if (i < mButtons.size())
                        return mButtons.at(i);

                    return 0;
                }

                const std::shared_ptr<Input::Joystick::Mappings::Mapping> getMapping() { return mMapping; }
                const Type getType() { return mType; }

                void setJoystickIsPresent(bool x) { mJoystickIsPresent = x; }
                void setJoystickWasPresent(bool x) { mJoystickWasPresent = x; }
                void setJoystickAxesCount(unsigned x) { mJoystickAxesCount = x; }

                void setJoystickButtonsCount(unsigned x)
                {
                    mJoystickButtonsCount = x;

                    mButtons.resize(mJoystickButtonsCount);
                    std::fill(mButtons.begin(), mButtons.end(), false);
                }

                void setJoystickId(unsigned x) { mJoystickId = x; }
                void setName(const char *x) { mName = x; }
                void setJoystickAxes(const float *x) { mJoystickAxes = x; }
                void setJoystickButtons(const unsigned char *x) { mJoystickButtons = x; }
                void setJoystickButton(unsigned x, bool state) { mButtons.at(x) = state; }
                void setType(Type x) { mType = x; }
                void setMapping(std::shared_ptr<Input::Joystick::Mappings::Mapping> mapping) { mMapping = mapping; }

            private:
                bool mJoystickIsPresent = false;
                bool mJoystickWasPresent = false;
                int mJoystickAxesCount = 0;
                int mJoystickButtonsCount = 0;
                int mJoystickId = -1;
                const char *mName = nullptr;
                const float *mJoystickAxes = nullptr;
                const unsigned char *mJoystickButtons = nullptr;
                std::vector<bool> mButtons;
                std::shared_ptr<Input::Joystick::Mappings::Mapping> mMapping;
                Type mType;
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
                    try
                    {
                        if (JoystickVector.size() > 0)
                        {
                            if (JoystickVector.find(0) != JoystickVector.end())
                            {
                                if (JoystickVector.at(0)->getJoystickIsPresent())
                                {
                                    return JoystickVector.at(0);
                                }
                            }
                        }

                        return dummyJoystick;
                    }
                    catch (std::exception &e)
                    {
                        std::cout << "Exception caught at: " << __FUNCTION__ << "\n line: " << __LINE__ << ".\n"
                                  << "What(): " << e.what() << std::endl;
                    }

                    return nullptr;
                }

                static void JoystickCallbackGLFW(int joystick, int event)
                {
                    try
                    {
                        if (event == GLFW_DISCONNECTED)
                        {
                            if (JoystickVector.find(joystick) != JoystickVector.end())
                            {
                                JoystickVector.at(joystick)->setJoystickIsPresent(false);
                                JoystickVector.erase(joystick);
                                std::cout << "Joystick Disconnected" << std::endl;
                                std::cout << "Joystick vector size: " << JoystickVector.size() << std::endl;
                            }
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
                            if (glfwJoystickPresent(i) && JoystickVector.find(i) == JoystickVector.end())
                            {
                                std::string lName = glfwGetJoystickName(i);
                                Joystick_ptr _joystick = std::make_shared<Joystick>();
                                namespace _Mappings = Input::Joystick::Mappings;
                                //Assign Controller specific mapping and type
                                int buttonsCount;
                                glfwGetJoystickButtons(i, &buttonsCount);
                                if (lName.find(Input::Joystick_Name::XBOX360) != std::string::npos && buttonsCount == Input::XBOX360_BUTTONCOUNT)
                                {
                                    std::cout << "Xbox 360 Controller mapping selected" << std::endl;
                                    _joystick->setMapping(std::make_shared<Input::Joystick::Mappings::Xbox>());
                                    _joystick->setType(Input::Joystick::XBOX360);
                                }
                                else if (lName.find(Input::Joystick_Name::XBOXONE) != std::string::npos && buttonsCount == Input::XBOXONE_BUTTONCOUNT)
                                {
                                    std::cout << "Xbox One Controller mapping selected" << std::endl;
                                    _joystick->setMapping(std::make_shared<Input::Joystick::Mappings::Xbox>());
                                    _joystick->setType(Input::Joystick::XBOXONE);
                                }
                                else if (lName.find(Input::Joystick_Name::DUALSHOCK3) != std::string::npos && buttonsCount == Input::DUALSHOCK3_BUTTONCOUNT)
                                {
                                    std::cout << "Dualshock3 Controller mapping selected" << std::endl;
                                    _joystick->setMapping(std::make_shared<Input::Joystick::Mappings::DualShock3>());
                                    _joystick->setType(Input::Joystick::DUALSHOCK3);
                                }
                                else if (lName.find(Input::Joystick_Name::DUALSHOCK4) != std::string::npos && buttonsCount == Input::DUALSHOCK4_BUTTONCOUNT)
                                {
                                    std::cout << "Dualshock4 Controller mapping selected" << std::endl;
                                    _joystick->setMapping(std::make_shared<Input::Joystick::Mappings::DualShock4>());
                                    _joystick->setType(Input::Joystick::DUALSHOCK4);
                                }
                                else
                                {
                                    std::cout << "Generic Controller mapping selected" << std::endl;
                                    _joystick->setMapping(std::make_shared<Input::Joystick::Mappings::Generic>());
                                    _joystick->setType(Input::Joystick::GENERIC);
                                }
                                std::cout << "New joystick connected: " << lName << std::endl;
                                _joystick->setJoystickIsPresent(true);
                                _joystick->setJoystickWasPresent(true);
                                _joystick->setJoystickId(i);
                                _joystick->setName(lName.c_str());
                                int axesCount;
                                _joystick->setJoystickAxes(glfwGetJoystickAxes(i, &axesCount));
                                std::cout << "Axis Count: " << axesCount << std::endl;

                                _joystick->setJoystickButtons(glfwGetJoystickButtons(i, &buttonsCount));
                                std::cout << "Axis Count: " << buttonsCount << std::endl;
                                _joystick->setJoystickButtonsCount(buttonsCount);
                                _joystick->setJoystickAxesCount(axesCount);

                                JoystickVector[i] = _joystick;
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
                        for (std::size_t i = 0; i < JoystickVector.size(); i++)
                        {
                            if (JoystickVector.at(i)->getJoystickIsPresent() && JoystickVector.find(i) != JoystickVector.end())
                            {
                                if (JoystickVector.at(i) != nullptr)
                                {
                                    Joystick_ptr _joystick = JoystickVector.at(i);
                                    int axesCount, buttonsCount;
                                    _joystick->setJoystickAxes(glfwGetJoystickAxes(_joystick->getJoystickId(), &axesCount));
                                    _joystick->setJoystickButtons(glfwGetJoystickButtons(_joystick->getJoystickId(), &buttonsCount));

                                    for (unsigned int k = 0; k < _joystick->getJoystickButtonsCount(); k++)
                                    {
                                        if (_joystick->getJoystickButtons()[k] == GLFW_PRESS)
                                        {
                                            _joystick->setJoystickButton(k, true);
                                            std::cout << k << " pressed" << std::endl;
                                        }
                                        else if (_joystick->getJoystickButtons()[k] == GLFW_RELEASE)
                                        {
                                            _joystick->setJoystickButton(k, false);
                                        }
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
                static std::unordered_map<unsigned, Joystick_ptr> JoystickVector;
            };

        } // namespace Joystick
    }     // namespace Input
}
