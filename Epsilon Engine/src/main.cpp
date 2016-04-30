///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <iostream>
#include <Shader.h>
#include <SOIL.h>
#include <main.h>
#include <exception>
#include <assert.h>
#include <memory>
using namespace std;

int main(int argc, char* argv[])
{
    GLFWwindow* window = InitEngine("Epsilon Engine");

    std::unique_ptr<App> MainProgram = (std::unique_ptr<App>) (new App(window));

    MainProgram->Run();

    glfwTerminate();

    return 0;
}
