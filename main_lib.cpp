#include <string>
#include <core/engine/context.hpp>
#include <Epsilon.hpp>
#include "core/framework/singleton.hpp"

#ifndef BUILD_ANDROID
#include <apis/vk/vk.hpp>

int lib_test()
{
    return 0;
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    static Epsilon::Epsilon instance;

    EXPORT void EpsilonInit(const char *name)
    {
        std::cout << "Creating epsilon instance: " << name << std::endl;
        //auto &instance = Epsilon::Epsilon::getSingleton();
        instance.m_ApplicationName = name;
    }

#ifdef CreateWindow
#undef CreateWindow
#endif

    [[maybe_unused]] EXPORT void CreateWindow(const char *name, int w, int h)
    {
        instance.getContext()->Window().init(name, w, h);
        instance.m_ApplicationName = name;
    }

    EXPORT void CreateContext(const char *name, engine::renderer_type api)
    {
        instance.getContext()->Init(name, api);
    }

    EXPORT void EpsilonRun()
    {
        instance.run();
    }
#ifdef __cplusplus
};
#endif