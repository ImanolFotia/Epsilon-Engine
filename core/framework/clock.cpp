//
// Created by solaire on 2/2/23.
//

#include "clock.hpp"


long double framework::Clock::mCurrentTime = 0;
long double framework::Clock::mLastTime = 0;
long double framework::Clock::mDeltaTime = 0;

std::chrono::steady_clock::time_point framework::Clock::mStart{};
long double framework::Clock::mdStart = 0.0;