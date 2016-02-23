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
using namespace std;

int main(int argc, char* argv[])
{
    GLFWwindow* window = InitEngine("Epsilon Engine");

    App* MainProgram = new App(window);

    MainProgram->Run();

    delete MainProgram;
    //glfwDestroyWindow(window);
    delete window;

    return 0;
}
