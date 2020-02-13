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
    GLFWwindow* window = Init("Epsilon Engine");

    std::unique_ptr<App> MainProgram = std::make_unique<App>(window);

    MainProgram->Run();

    glfwTerminate();

    return 0;
}
