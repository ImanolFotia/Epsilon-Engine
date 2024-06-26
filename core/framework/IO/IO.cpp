//
// Created by solaire on 12/23/22.
//

#include "Mouse.hpp"
#include "KeyBoard.hpp"
#include "DragDrop.hpp"
#include "core/framework/IO/Joystick/Joystick.hpp"

double framework::Input::Mouse::XPOS = 500;
double framework::Input::Mouse::YPOS = 500;

double framework::Input::Mouse::prevxOffset = 0.0;
double framework::Input::Mouse::prevyOffset = 0.0;

framework::Input::STATE framework::Input::Mouse::LEFT = Input::NONE;
framework::Input::STATE framework::Input::Mouse::MIDDLE = Input::NONE;
framework::Input::STATE framework::Input::Mouse::RIGHT = Input::NONE;

// Epsilon::Input::MouseArgs Epsilon::Input::Mouse::mouseArgs;

// beacon::single_handler<framework::Input::KeyboardArgs> framework::Input::KeyBoard::KeyboardEventHandler;

// beacon::single_handler<framework::Input::DropArgs> framework::Input::DragDrop::DropEventHandler;

namespace Joystick = framework::Input::Joystick;

// std::shared_ptr<framework::Input::Joystick::Joystick> framework::Input::Joystick::JoystickManager::dummyJoystick = std::make_shared<framework::Input::Joystick::Joystick>();

// std::unordered_map<unsigned, Joystick::JoystickManager::Joystick_ptr> Joystick::JoystickManager::JoystickVector;
