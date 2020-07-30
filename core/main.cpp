///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <pch.hpp>

#include <App.hpp>
#include <Init.hpp>

#include <Platform/WindowBase.hpp>

int main(int argc, char* argv[])
{
    std::shared_ptr<Epsilon::Platform::WindowBase> window = Epsilon::Init("Epsilon Engine");

    std::shared_ptr<Epsilon::App> MainProgram = std::make_shared<Epsilon::App>(window);

    MainProgram->Run();

    glfwTerminate();

    return 0;
}
