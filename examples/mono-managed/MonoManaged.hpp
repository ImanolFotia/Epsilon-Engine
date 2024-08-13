//
// Created by solaire on 2/3/23.
//

#ifndef EPSILON_MONOMANAGED_HPP
#define EPSILON_MONOMANAGED_HPP

#include <Epsilon.hpp>

static void CreateWindow(MonoString *name, int w, int h);

namespace MonoManaged
{
    class MonoManaged : public Epsilon::Epsilon
    {

        modules::mono::MonoInstance m_pMonoInstance;

    public:
        MonoManaged() : Epsilon::Epsilon("MonoManaged")
        {
            Epsilon::getSingleton().onReady = [this]
            { onReady(); };
            Epsilon::getSingleton().onCreate = [this]
            { onCreate(); };
            Epsilon::getSingleton().onRender = [this]
            { onRender(); };
            Epsilon::getSingleton().onExit = [this]
            { onExit(); };

            auto domain = m_pMonoInstance.addDomain("Main", "assets/scripts/test.dll");
            m_pMonoInstance.mapInternalCall("Epsilon.EpsilonBehaviour::CreateWindow", (const void *)&CreateWindow);
            domain.Execute();
        }

        void onReady() {}
        void onCreate() {}
        void onRender() {}
        void onExit() {}
    };
}
// template <>
// engine::Context singleton<MonoManaged::MonoManaged>::self{};

#endif // EPSILON_MONOMANAGED_HPP
