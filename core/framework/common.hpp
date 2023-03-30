#pragma once

#include <iostream>
#include <ctime>   
#include <chrono>

#ifdef _MSC_VER
#define EPSILON_API  __declspec(dllexport)
#else
#define EPSILON_API __attribute__((visibility("default")))
#endif

class IO
{
public:
    template <class... Args>
    static void Log(Args &&...args)
    {
        std::clog << "\033[1;37mlog: ";
        (std::clog << ... << args) << std::endl;
    }

    template <class... Args>
    static void Info(Args &&...args)
    {
        std::clog << "\033[1;34minfo: ";
        (std::clog <<  ... << args) << "\033[0m" << std::endl;
    }

    template <class... Args>
    static void Warning(Args &&...args)
    {
        std::clog << "\033[1;33mwarning: ";
        (std::clog <<  ... << args) << "\033[0m" << std::endl;
    }

    template <class... Args>
    static void Error(Args &&...args)
    {
        std::clog << "\033[1;31merror: ";
        (std::clog <<  ... << args) << "\033[0m" << std::endl;
    }
};