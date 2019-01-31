#pragma once

#include <cmath>

template <typename R>
struct SH
{
    SH(uint32_t a_t, uint32_t a_l, double a_d = 0.0)
    : t(a_t)
    , l(a_l)
    , d(a_d)
    {
    }

    SH Sin(double f)
    {
        return SH<R>(t, l, d + std::sin((double)t / f));
    }

    SH Vol(double v)
    {
        return SH<R>(t, l, d * (std::numeric_limits<int16_t>::max() * v));
    }

    R Done()
    {
        return (R)d;
    }

    uint32_t t;
    uint32_t l;
    double d;
};
