#pragma once

#include <iostream>

namespace Epsilon
{
    namespace IO
    {

        template <typename T>
        static inline void Print(const T &in)
        {
            std::cout << in;
        }

        template <typename T, class... Args>
        static inline void Print(const T &in, Args... args)
        {
            std::cout << in << " ";
            Print(args...);
        }
        
        template <typename T>
        static inline void PrintLine(const T &in)
        {
            std::cout << in << "\n";
        }

        template <typename T, class... Args>
        static inline void PrintLine(const T &in, Args... args)
        {
            std::cout << in << " ";
            PrintLine(args...);
        }
    } // namespace IO
} // namespace Epsilon