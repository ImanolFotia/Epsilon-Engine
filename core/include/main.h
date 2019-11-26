///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <ProgramData.h>
#include <Epsilon.h>
#include <memory>
#include <Log.h>
class App {
public:

    App(GLFWwindow*& win) {
        EpsilonEngine = (std::unique_ptr<Epsilon>) (new Epsilon(win));

        EpsilonEngine->InitResources();
    }

    void Run(void) {
        std::cout << "==================================================" << std::endl;
        std::cout << "Engine Initialization Complete." << std::endl;
        std::cout << "Now Rendering." << std::endl;

        EpsilonEngine->MainLoop();
    }


    ~App(void) {
        std::cout << "Engine shuted down" << std::endl;
    }

public:
    std::unique_ptr<Epsilon> EpsilonEngine;

};

std::ofstream Global::Log::FILE;
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
GLFWwindow* InitEngine(const char* ProgramName) {

    Global::Log::OpenFile("log.txt");

    std::cout << "Initializing Epsilon Engine" << std::endl;
    Global::Log::WriteToLog("Initializing Epsilon Engine");

    ProgramData DATA;
    std::cout << ProgramName << std::endl;

    if(!glfwInit()) {
        std::cout << "GLFW could not be initialized" << std::endl;
        Global::Log::WriteToLog("GLFW could not be initialized");
    }
    glfwSetErrorCallback(error_callback);
    /// glfwWindowHint(GLFW_SAMPLES, DATA.MSAA_SAMPLES);

    int numberofmonitors;

    GLFWmonitor** Monitor;
    GLFWmonitor* CurrentMonitor;

    Monitor = glfwGetMonitors(&numberofmonitors);

    for(int i = 0; i < numberofmonitors; ++i)
    {
       // std::cout << "-----------------------------------" << std::endl;
       // std::cout << glfwGetMonitorName(Monitor[i]) << std::endl;
        int numberofmodes;
        const GLFWvidmode* modes = glfwGetVideoModes(Monitor[i], &numberofmodes);
       // for(int j = 0; j < numberofmodes;++j)
       // {
       //     std::cout << modes[j].width << "x" << modes[j].height << std::endl;
       // }
        int widthMM, heightMM;
        glfwGetMonitorPhysicalSize(Monitor[i], &widthMM, &heightMM);
        const GLFWvidmode* mode = glfwGetVideoMode(Monitor[i]);
        const double dpi = mode->width / (widthMM / 25.4);
       // std::cout << "Monitor physical size: " << widthMM << "x" << heightMM << std::endl;
        //std::cout << "DPI: " << dpi << std::endl;
      // std::cout << "-----------------------------------" << std::endl;
    }

    if(numberofmonitors < DATA.MONITOR)
        CurrentMonitor = glfwGetPrimaryMonitor();
    else
        CurrentMonitor = Monitor[DATA.MONITOR];

    int numberofmodes;

    const GLFWvidmode* modes = glfwGetVideoModes(CurrentMonitor, &numberofmodes);

    GLFWwindow* window;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


    const GLFWvidmode* mode = glfwGetVideoMode(CurrentMonitor);
    //glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    //glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    //glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    //glfwWindowHint(GLFW_REFRESH_RATE, 60);
    //glfwWindowHint(GLFW_RED_BITS, 16);
    //glfwWindowHint(GLFW_GREEN_BITS, 16);
    //glfwWindowHint(GLFW_BLUE_BITS, 16);

    cout << "Creating window || Window Resolution: ";
    if(DATA.FULLSCREEN == 1) {
        window = glfwCreateWindow(DATA.WINDOW_WIDTH, DATA.WINDOW_HEIGHT, ProgramName, CurrentMonitor, 0);
        cout << DATA.WINDOW_WIDTH << " x " << DATA.WINDOW_HEIGHT << endl;
    } else if(DATA.FULLSCREEN == 2) {
        mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_DECORATED, GL_FALSE);
        window = glfwCreateWindow(mode->width, mode->height, ProgramName, CurrentMonitor, 0);
        cout << mode->width << " x " << mode->height << endl;
    } else {
        cout << DATA.WINDOW_WIDTH << " x " << DATA.WINDOW_HEIGHT << endl;
        window = glfwCreateWindow(DATA.WINDOW_WIDTH, DATA.WINDOW_HEIGHT, ProgramName, nullptr, nullptr);
        glfwSetWindowPos(window, (mode->width/2) - (DATA.WINDOW_WIDTH*0.5), (mode->height/2) - (DATA.WINDOW_HEIGHT*0.5));
    }

    glfwMakeContextCurrent(window);

    if(!window) {
        std::cout << "Window could not be created" << std::endl;
        Global::Log::WriteToLog("Window could not be created");
    }


    glewExperimental = GL_TRUE;
    glewInit();
    int width, height, channel;
    unsigned char* pixels = SOIL_load_image("resources/ui/cursor32.png", &width, &height, &channel, SOIL_LOAD_RGBA);
    std::cout << channel << std::endl;
    GLFWimage cursorImage;
    cursorImage.width = width;
    cursorImage.height = height;
    cursorImage.pixels = pixels;

    GLFWcursor* cursor = glfwCreateCursor(&cursorImage, 0, 0);
    glfwSetCursor(window, cursor);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSetWindowPos(window, 0, 0);
    glfwSetKeyCallback(window, Input::KeyBoard::KeyBoardCallBack);
    glfwSetCursorPosCallback(window, Input::Mouse::MouseCallBack);
    glfwSetJoystickCallback(Input::Joystick::JoystickCallback);
    //std::cout << "New joystick connected: " << glfwGetJoystickName(GLFW_JOYSTICK_1) << std::endl;

    glfwJoystickPresent(GLFW_JOYSTICK_1);
    glfwJoystickPresent(GLFW_JOYSTICK_2);
    glfwJoystickPresent(GLFW_JOYSTICK_3);
    glfwJoystickPresent(GLFW_JOYSTICK_4);
    glfwJoystickPresent(GLFW_JOYSTICK_5);

    glfwSwapInterval(DATA.VSYNC);

    //delete[] Monitor;
    //delete CurrentMonitor;
    //delete modes;

    return window;
}

#endif /// MAIN_H_INCLUDED
