///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <App.hpp>
#include <Init.hpp>
#include <memory>

int main(int argc, char* argv[])
{
    GLFWwindow* window = Epsilon::Init("Epsilon Engine");

    std::unique_ptr<Epsilon::App> MainProgram = std::make_unique<Epsilon::App>(window);

    MainProgram->Run();

    glfwTerminate();

    return 0;
}
