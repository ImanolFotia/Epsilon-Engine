///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#pragma once
#include <Core.hpp>
#include <ProgramData.h>
#include <Epsilon.h>
#include <memory>
#include <Log.h>

namespace Epsilon
{
    class App
    {
    public:
        App(std::shared_ptr<Platform::WindowBase> win)
        {
            EpsilonEngine = std::make_shared<Epsilon>(win);

            EpsilonEngine->InitResources();
        }

        void Run()
        {
            std::cout << "==================================================" << std::endl;
            std::cout << "Engine Initialization Complete." << std::endl;
            std::cout << "Now Rendering." << std::endl;

            EpsilonEngine->MainLoop();
        }

        ~App()
        {
            std::cout << "Engine shutted down" << std::endl;
        }

    public:
        std::shared_ptr<Epsilon> EpsilonEngine;
    };
} // namespace Epsilon
