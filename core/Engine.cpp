#include <Engine.hpp>
#include <Log.h>
#include <Clock.hpp>
#include <Resource/Resource.hpp>
#include <Platform/WindowBase.hpp>

namespace Epsilon
{

    Engine Engine::instance;
    std::ofstream Log::sOutFile;
    Clock Clock::instance;
    uint32_t Resources::ResourceInternal::gResourceIndex = 0;

    bool Platform::WindowBase::mNeedsToResize = false;
    Platform::WindowData Platform::WindowBase::mWindowData = {};
} // namespace Epsilon