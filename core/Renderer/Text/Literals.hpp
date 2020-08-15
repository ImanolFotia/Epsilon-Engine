#pragma once

namespace Epsilon
{
    namespace Text
    {
        namespace Literals
        {

            constexpr long double operator"" _em(long double a)
            {
                return a * 16;
            }

            constexpr unsigned long long int operator"" _em(unsigned long long int a)
            {
                return a * 16;
            }

            constexpr long double operator"" _per(long double a)
            {
                return ((float)a / 100.0) * 16;
            }

            constexpr unsigned long long int operator"" _per(unsigned long long int a)
            {
                return ((float)a / 100.0) * 16;
            }

            constexpr long double operator"" _pt(long double a)
            {
                return ((float)a / 12.0) * 16;
            }

            constexpr unsigned long long int operator"" _pt(unsigned long long int a)
            {
                return ((float)a / 12.0) * 16;
            }
        } // namespace Literals
    } // namespace Text
} // namespace Epsilon