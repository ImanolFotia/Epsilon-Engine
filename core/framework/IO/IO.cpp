//
// Created by solaire on 12/23/22.
//

#include "Mouse.hpp"
#include "KeyBoard.hpp"
#include "core/framework/IO/Joystick/Joystick.hpp"


double framework::Input::Mouse::XPOS = 500;
double framework::Input::Mouse::YPOS = 500;

double framework::Input::Mouse::prevxOffset = 0.0;
double framework::Input::Mouse::prevyOffset = 0.0;

framework::Input::STATE framework::Input::Mouse::LEFT = Input::RELEASED;
framework::Input::STATE framework::Input::Mouse::MIDDLE = Input::RELEASED;
framework::Input::STATE framework::Input::Mouse::RIGHT = Input::RELEASED;

//Epsilon::Input::MouseArgs Epsilon::Input::Mouse::mouseArgs;

beacon::single_handler<framework::Input::MouseArgs> framework::Input::Mouse::MouseEventHandler;

beacon::single_handler<framework::Input::KeyboardArgs> framework::Input::KeyBoard::KeyboardEventHandler;

namespace Joystick = framework::Input::Joystick;

std::unordered_map<unsigned, Joystick::JoystickManager::Joystick_ptr> Joystick::JoystickManager::JoystickVector;
Joystick::JoystickManager::Joystick_ptr Joystick::JoystickManager::dummyJoystick = std::make_shared<Input::Joystick::Joystick>();
