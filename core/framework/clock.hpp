#pragma once

#include <chrono>
#include <iostream>

namespace framework {


class Clock {
    public:

        Clock(const Clock &) = delete;
        Clock(Clock &&) = delete;

        static long double Time() {
            return instance.curr();
        }
        
        static long double TimeSeconds() {
            return instance.curr() / 1000.0;
        }

        static long double Last() {
            return instance.last();
        }
        
        static long double LastSeconds() {
            return instance.last() / 1000.0;
        }

        static long double Delta() {
            return instance.delta();
        }
        
        static long double DeltaSeconds() {
            return instance.delta() / 1000.0;
        }

        static auto Now() {
            using namespace std::chrono_literals;
            auto n = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());

            return (n - std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>(0us)).count();
        }

        static void Tick() {
            using namespace std::chrono; 

            auto now = steady_clock::now();
            auto now_ms = time_point_cast<milliseconds>(now);

            instance.last(instance.curr());
            instance.curr(duration_cast<milliseconds>(now_ms - instance.start()).count());
            instance.delta(instance.curr() - instance.last());

        }

    private:

        long double curr() { return mCurrentTime; }
        long double last() { return mLastTime; }
        long double delta() { return mDeltaTime; }
        
        std::chrono::steady_clock::time_point start() { return mStart; }
        
        void curr(long double x) { mCurrentTime = x; }
        void last(long double x) { mLastTime = x; }
        void delta(long double x) { mDeltaTime = x; }

        long double mCurrentTime = 0;
        long double mLastTime = 0;
        long double mDeltaTime = 0;

        std::chrono::steady_clock::time_point mStart{};
        long double mdStart = 0.0;

        Clock() {
            std::cout << "created" << std::endl;
            using namespace std::chrono; 
            mCurrentTime = 1;
            mLastTime = 0;
            mDeltaTime = 0;
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            mStart = time_point_cast<milliseconds>(steady_clock::now());

        }

        static Clock instance;
    };

}