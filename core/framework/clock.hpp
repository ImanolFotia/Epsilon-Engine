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
#if defined(EPSILON_BUILD_DLL) && !defined(EPSILON_DLL)
#define EPSILON_DLL __attribute__((visibility("default")))
#else
#if !defined(EPSILON_DLL)
#define EPSILON_DLL
#endif
#endif
#else
//  do nothing and hope for the best?
#if !defined(EPSILON_DLL)
#define EPSILON_DLL
#endif
#pragma warning Unknown dynamic link import / export semantics.
#endif

namespace framework
{

    class EPSILON_DLL Clock
    {
    public:
        Clock(const Clock &) = delete;
        Clock(Clock &&) = delete;

        static long double Time();

        static long double TimeSeconds();

        static long double Last();

        static long double LastSeconds();

        static long double Delta();

        static long double DeltaSeconds();

        static uint64_t Now();

        static void Tick();

    private:
        static long double curr();
        static long double last();
        static long double delta();

        static std::chrono::steady_clock::time_point start();

        static void curr(long double x);
        static void last(long double x);
        static void delta(long double x);

        static long double mCurrentTime;
        static long double mLastTime;
        static long double mDeltaTime;

        static std::chrono::steady_clock::time_point mStart;
        static long double mdStart;

        Clock()
        {
            std::cout << "created" << std::endl;
            using namespace std::chrono;
            mCurrentTime = 1;
            mLastTime = 0;
            mDeltaTime = 0;
            mStart = time_point_cast<milliseconds>(steady_clock::now());
        }
    };

}