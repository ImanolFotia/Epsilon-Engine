#include <Engine.hpp>
#include <Log.h>
#include <Clock.hpp>


namespace Epsilon {
    
    Engine Engine::instance;
    std::ofstream Log::sOutFile;
    Clock Clock::instance;
}