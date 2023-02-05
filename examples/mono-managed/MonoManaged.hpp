//
// Created by solaire on 2/3/23.
//

#ifndef EPSILON_MONOMANAGED_HPP
#define EPSILON_MONOMANAGED_HPP

#include <Epsilon.hpp>
namespace MonoManaged {
    class MonoManaged : public Epsilon::Epsilon {

        modules::mono::MonoInstance m_pMonoInstance;

    public:

        MonoManaged() : Epsilon::Epsilon("MonoManaged") {
            Epsilon::getSingleton().onReady = [this]{ onReady(); };
            Epsilon::getSingleton().onCreate = [this]{ onCreate(); };
            Epsilon::getSingleton().onRender = [this]{ onRender(); };
            Epsilon::getSingleton().onExit = [this]{ onExit(); };

            auto domain = m_pMonoInstance.addDomain("Main", "assets/scripts/test.dll");
            modules::mono::MonoInstance::getSingleton().mapInternalCall("Epsilon.EpsilonBehaviour::CreateWindow", (const void*) &CreateWindow);
            domain.Execute();
        }

        void onReady() {}
        void onCreate() {}
        void onRender() {}
        void onExit() {}
    };
}

#endif //EPSILON_MONOMANAGED_HPP
