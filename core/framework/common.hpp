#pragma once

#include <iostream>
#include <ctime>   
#include <chrono>

#ifdef _MSC_VER
#define EPSILON_API  __declspec(dllexport)
#else
#define EPSILON_API __attribute__((visibility("default")))
#endif

#if defined(ANDROID) || defined(__ANDROID__)

#include <android/log.h>

#define LOG_TAG "Epsilon"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

inline std::string const& to_string(std::string const& s) { return s; }

template<typename... Args>
std::string stringer(Args const&... args)
{
    std::string result;
    using ::to_string;
    using std::to_string;
    int unpack[]{ 0, (result += to_string(args), 0)... };
    static_cast<void>(unpack);
    return result;
}
#endif

class IO
{
public:
    template <class... Args>
    static void Log(Args &&...args)
    {
        std::clog << "\033[1;37mlog: ";
        (std::clog << ... << args) << std::endl;
#if defined(ANDROID) || defined(__ANDROID__)
        LOGI("%s", stringer(args...).c_str());
#endif
    }

    template <class... Args>
    static void Info(Args &&...args)
    {
        std::clog << "\033[1;34minfo: ";
        (std::clog <<  ... << args) << "\033[0m" << std::endl;
#if defined(ANDROID) || defined(__ANDROID__)
        LOGI("%s", stringer(args...).c_str());
#endif
    }

    template <class... Args>
    static void Warning(Args &&...args)
    {
        std::clog << "\033[1;33mwarning: ";
        (std::clog <<  ... << args) << "\033[0m" << std::endl;
#if defined(ANDROID) || defined(__ANDROID__)
        LOGI("%s", stringer(args...).c_str());
#endif
    }

    template <class... Args>
    static void Error(Args &&...args)
    {
        std::clog << "\033[1;31merror: ";
        (std::clog <<  ... << args) << "\033[0m" << std::endl;
#if defined(ANDROID) || defined(__ANDROID__)
        LOGE("%s", stringer(args...).c_str());
#endif
    }
};