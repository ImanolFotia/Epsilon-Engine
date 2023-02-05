#include <string>
#include <core/engine/context.hpp>
#include <Epsilon.hpp>
#include "core/framework/singleton.hpp"

#if defined(_WIN32)
//  Microsoft
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__linux__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif


#ifndef BUILD_ANDROID
#include <apis/vk/vk.hpp>

int lib_test() {
    return 0;
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void EpsilonInit(const char* name) {
    std::cout << "Creating epsilon instance: " << name << std::endl;
    auto& instance = Epsilon::Epsilon::getSingleton();
    instance.m_ApplicationName = name;
}

[[maybe_unused]] EXPORT void CreateWindow(const char* name, int w, int h) {
    engine::Context::getSingleton().Window().init(name, w, h);
    Epsilon::Epsilon::getSingleton().m_ApplicationName = name;
}

EXPORT void CreateContext(const char* name, engine::renderer_type api) {
    engine::Context::getSingleton().Init(name, api);
}

EXPORT void EpsilonRun() {
    Epsilon::Epsilon::getSingleton().run();
}
#ifdef __cplusplus
};
#endif