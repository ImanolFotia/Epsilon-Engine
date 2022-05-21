#pragma once

#include <iostream>

class IO
{
public:
    template <class... Args>
    static void Log(Args &&...args)
    {
        (std::cout << ... << args) << std::endl;
    }
};