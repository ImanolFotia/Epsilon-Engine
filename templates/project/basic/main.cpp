//
// Created by solaire on 12/15/22.
//

#include <iostream>

#include "TowerDefense.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef min
#undef max
#include <windows.h>
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#endif

// Should force the OS to use the dedicated GPU, however, it doesn't work while using hybrid graphics on Pop!_OS, so the command must be run using:
// For OpenGL:
//__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia <application>
// For Vulkan:
//__NV_PRIME_RENDER_OFFLOAD=1 <application>

#ifdef __linux__
extern "C"
{
    int NvOptimusEnablement = 1;
    int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int main(int argc, char **argv)
{

    framework::env::setArgs(argc, argv);

    Template::Template app("Template");

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    return 0;
}