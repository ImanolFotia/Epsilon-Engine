///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <pch.hpp>

#include <Engine.hpp>
#include <Epsilon.h>


int main(int argc, char* argv[])
{
    Epsilon::Engine::Get().ParseArguments(argc, argv);

    Epsilon::Epsilon MainProgram("Epsilon Engine");

    MainProgram.Begin();

    MainProgram.Run();

    return 0;
}
