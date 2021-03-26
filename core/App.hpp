///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#pragma once
#include <pch.hpp>
#include <Core.hpp>
#include <ProgramData.h>
#include <Log.h>

#include <Platform/WindowBase.hpp>

namespace Epsilon
{
    class App
    {
    public:
        App(const char* appName);

        App() = default;

        void Begin();

        void Run();

        ~App();

    public:
        virtual void onCreate() = 0;
        virtual void onRender() = 0;
        virtual void onExit() = 0;

    protected:
        std::shared_ptr<Platform::WindowBase> mWindow;
    };
} // namespace Epsilon
