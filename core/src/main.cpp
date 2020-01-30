///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <main.h>
#include <memory>

int main(int argc, char* argv[])
{
    GLFWwindow* window = InitEngine("Epsilon Engine");

    std::unique_ptr<App> MainProgram = (std::unique_ptr<App>) (new App(window));

    MainProgram->Run();

    glfwTerminate();

    return 0;
}
