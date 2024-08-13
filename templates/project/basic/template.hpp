#pragma once

#include <Epsilon.hpp>
#include <beacon/beacon.hpp>

namespace Template
{
    class Template : public Epsilon::Epsilon
    {

    public:
        explicit Template(const std::string &appName) : Epsilon::Epsilon(appName)
        {

            Epsilon::getSingleton().onCreate = [this]
            { onCreate(); };
            Epsilon::getSingleton().onReady = [this]
            { onReady(); };
            Epsilon::getSingleton().onRender = [this]
            { onRender(); };
            Epsilon::getSingleton().onExit = [this]
            { onExit(); };
        }

        void onCreate() {}

        void onReady() {}

        void onRender() {}

        void onExit() {}
    };
}