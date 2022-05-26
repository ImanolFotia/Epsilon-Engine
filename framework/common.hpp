#pragma once

#include <iostream>
#include <ctime>   
#include <chrono>

class IO
{
public:
    template <class... Args>
    static void Log(Args &&...args)
    {
        (std::clog << ... << args) << std::endl;
    }
};