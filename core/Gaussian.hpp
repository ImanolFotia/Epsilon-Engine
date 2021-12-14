//Helper function to create gaussian kernels
#pragma once

#include <glm/glm.hpp>
#include <array>

namespace Epsilon
{
    static inline double GaussianFunction(double x, double a, double b, double c)
    {
        return a * glm::exp(-(((x * x) + (2*x*b) + (b*b)) / (2 * c * c)));
    }

    template <int N, typename T = double>
    static std::array<T, (N * 2) + 1> GaussianKernel1D(T a, T b, T c)
    {
        std::array<T, (N * 2) + 1> output;
        T index = -N - 1;
        T sum = 0.0;
        c = glm::max(0.0001f, c);
        for (auto &x : output)
        {
            x = GaussianFunction(++index, a, b, c);
            sum += x;
        }

        for (auto &x : output)
            x /= sum;

        return output;
    }
}
