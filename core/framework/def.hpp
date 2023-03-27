#pragma once

#include <numeric>
#include <glm/glm.hpp>

#ifdef MIN
#undef MIN
#undef MAX
#undef min
#undef max
#endif

constexpr double PI = 3.14159265359;
constexpr double HPI = 3.14159265359 * 0.5;
constexpr double TAU = PI * 0.5;

template <typename T>
auto static clamp(T a, T b, T c, double d = 0.0, double e = 1.0) -> T
{
    a = (a < b) ? (b + d) * e : a;
    return (a > c) ? (c + d) * e : a;
};

template <typename T>
auto static inRange(T x, T a, T b) -> bool
{
    return (x > a) && (x < b);
};

// Minimum value for any given type
template <typename T>
inline static T getMinimum()
{
    return std::numeric_limits<T>::min();
}

// Maximum value for any given type
template <typename T>
inline static T getMaximum()
{
    return std::numeric_limits<T>::max();
}

[[maybe_unused]] static glm::vec2 halton(int index)
{
    const glm::vec2 coprimes = glm::vec2(2.0f, 3.0f);
    glm::vec2 s = glm::vec2(index, index);
    glm::vec4 a = glm::vec4(1, 1, 0, 0);
    while (s.x > 0. && s.y > 0.)
    {
        glm::vec2 ar0 = glm::vec2(a.x, a.y) / coprimes;

        a.x = ar0.x;
        a.y = ar0.y;

        glm::vec2 ar1 = glm::vec2(a.x, a.y) * glm::mod(s, coprimes);

        a.z += ar1.x;
        a.w += ar1.y;
        s = glm::floor(s / coprimes);
    }
    return glm::vec2(a.z, a.w);
}

// Test device's endianess
static bool isBigEndian()
{
    int a = 1;
    return !((char *)&a)[0];
}

template <typename T>
T SwapEndian(T u)
{
    static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

static int ByteToInt(char *buffer, int len)
{
    int a = 0;
    if (!isBigEndian())
        for (int i = 0; i < len; i++)
            ((char *)&a)[i] = buffer[i];
    else
        for (int i = 0; i < len; i++)
            ((char *)&a)[3 - i] = buffer[i];
    return a;
}