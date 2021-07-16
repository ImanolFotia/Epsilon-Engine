#include <Engine.hpp>
#include <Log.h>
#include <Clock.hpp>
#include <Resource/Resource.hpp>
#include <Platform/WindowBase.hpp>
#include <Filesystem/Filesystem.hpp>

namespace Epsilon
{

    Engine Engine::instance;
    std::ofstream Log::sOutFile;
    Clock Clock::instance;
    Filesystem::Filesystem Filesystem::Filesystem::mInstance;
    uint32_t Resources::ResourceInternal::gResourceIndex = 0;

    bool Platform::WindowBase::mNeedsToResize = false;
    Platform::WindowData Platform::WindowBase::mWindowData = {};
} // namespace Epsilon  