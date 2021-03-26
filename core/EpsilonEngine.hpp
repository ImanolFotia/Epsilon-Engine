#pragma once

namespace Epsilon
{
    class EpsilonEngine
    {
        void Init() {
            
        }

        virtual void onCreate() = 0;

        virtual void onLoad() = 0;

        virtual void onRender() = 0;

        virtual void onExit() = 0;
    };
}