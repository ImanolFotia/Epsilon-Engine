//
// Created by solaire on 2/2/23.
//

#include "clock.hpp"

long double framework::Clock::mCurrentTime = 0;
long double framework::Clock::mLastTime = 0;
long double framework::Clock::mDeltaTime = 0;

std::chrono::steady_clock::time_point framework::Clock::mStart{};
long double framework::Clock::mdStart = 0.0;

namespace framework
{

    long double Clock::Time()
    {
        return curr();
    }

    long double Clock::TimeSeconds()
    {
        return curr() / 1000.0;
    }

    long double Clock::Last()
    {
        return last();
    }

    long double Clock::LastSeconds()
    {
        return last() / 1000.0;
    }

    long double Clock::Delta()
    {
        return delta();
    }

    long double Clock::DeltaSeconds()
    {
        return delta() / 1000.0;
    }

    uint64_t Clock::Now()
    {
        using namespace std::chrono_literals;
        auto n = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());

        return (n - std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>(0us)).count();
    }

    void Clock::Tick()
    {
        using namespace std::chrono;

        auto now = steady_clock::now();
        auto now_ms = time_point_cast<milliseconds>(now);

        last(curr());
        curr(duration_cast<milliseconds>(now_ms - start()).count());
        delta(curr() - last());
    }

    long double Clock::curr() { return mCurrentTime; }
    long double Clock::last() { return mLastTime; }
    long double Clock::delta() { return mDeltaTime; }

    std::chrono::steady_clock::time_point Clock::start() { return mStart; }

    void Clock::curr(long double x) { mCurrentTime = x; }
    void Clock::last(long double x) { mLastTime = x; }
    void Clock::delta(long double x) { mDeltaTime = x; }
}