///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#pragma once

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
        ResourceManager::Get().Destroy();
        std::cout << "Engine shuted down" << std::endl;
    }

public:
    std::unique_ptr<Epsilon> EpsilonEngine;

};


