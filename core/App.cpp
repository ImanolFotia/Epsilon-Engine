#include "App.hpp"

#include <Init.hpp>

#include <multithread/ThreadPool.hpp>
#include <Driver/API/OpenGL/GlCache.h>

#include <IO/KeyBoard.h>
#include <IO/Mouse.h>
#include <IO/Joystick/Joystick.h>


GLenum glCache::CullFaceMode = 0;
GLuint glCache::ShaderId = 0;
unsigned glCache::DrawCalls = 0;

using namespace std::placeholders;

double Epsilon::Input::Mouse::XPOS = 500;
double Epsilon::Input::Mouse::YPOS = 500;

double Epsilon::Input::Mouse::prevxOffset = 0.0;
double Epsilon::Input::Mouse::prevyOffset = 0.0;
 
Epsilon::Input::STATE Epsilon::Input::Mouse::LEFT = Input::RELEASED;
Epsilon::Input::STATE Epsilon::Input::Mouse::MIDDLE = Input::RELEASED;
Epsilon::Input::STATE Epsilon::Input::Mouse::RIGHT = Input::RELEASED;
 
//Epsilon::Input::MouseArgs Epsilon::Input::Mouse::mouseArgs;

beacon::single_handler<Epsilon::Input::MouseArgs> Epsilon::Input::Mouse::MouseEventHandler;
 
namespace Joystick = Epsilon::Input::Joystick;
 
std::unordered_map<unsigned, Joystick::JoystickManager::Joystick_ptr> Joystick::JoystickManager::JoystickVector;
Joystick::JoystickManager::Joystick_ptr Joystick::JoystickManager::dummyJoystick = std::make_shared<Input::Joystick::Joystick>();

//bool Epsilon::CheckBox::_checked = true;
ThreadPool::ThreadPool_ptr ThreadPool::_instance = nullptr;

namespace Epsilon
{
    App::App(const char *appName)
    {
        mWindow = Init(appName);
    }

    void App::Begin()
    {
        this->onCreate();
    }

    void App::Run()
    {
        std::cout << "==================================================" << std::endl;
        std::cout << "Engine Initialization Complete." << std::endl;
        std::cout << "Now Rendering." << std::endl;

        this->onRender();
    }

    App::~App()
    {
        mWindow->Destroy();
        std::cout << "Engine shutted down" << std::endl;
    }

}