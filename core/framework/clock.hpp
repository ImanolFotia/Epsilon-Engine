#pragma once

#include <chrono>
#include <iostream>

#if defined(_WIN32)
//  Microsoft
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __declspec(dllexport)
#else
#define EPSILON_DLL __declspec(dllimport)
#endif
#elif defined(__linux__)
//  GCC
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __attribute__((visibility("default")))
#else
#define EPSILON_DLL
#endif
#else
//  do nothing and hope for the best?
#define EPSILON_DLL
#define EPSILON_DLL
#pragma warning Unknown dynamic link import / export semantics.
#endif

namespace framework {


    class EPSILON_DLL Clock {
    public:

        Clock(const Clock&) = delete;
        Clock(Clock&&) = delete;

        static long double Time() {
            return curr();
        }

        static long double TimeSeconds() {
            return curr() / 1000.0;
        }

        static long double Last() {
            return last();
        }

        static long double LastSeconds() {
            return last() / 1000.0;
        }

        static long double Delta() {
            return delta();
        }

        static long double DeltaSeconds() {
            return delta() / 1000.0;
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

            last(curr());
            curr(duration_cast<milliseconds>(now_ms - start()).count());
            delta(curr() - last());

        }

    private:

        static long double curr() { return mCurrentTime; }
        static long double last() { return mLastTime; }
        static long double delta() { return mDeltaTime; }

        static std::chrono::steady_clock::time_point start() { return mStart; }

        static void curr(long double x) { mCurrentTime = x; }
        static void last(long double x) { mLastTime = x; }
        static void delta(long double x) { mDeltaTime = x; }

        static long double mCurrentTime;
        static long double mLastTime;
        static long double mDeltaTime;

        static std::chrono::steady_clock::time_point mStart;
        static long double mdStart;

        Clock() {
            std::cout << "created" << std::endl;
            using namespace std::chrono;
            mCurrentTime = 1;
            mLastTime = 0;
            mDeltaTime = 0;
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            mStart = time_point_cast<milliseconds>(steady_clock::now());

        }

    };

}