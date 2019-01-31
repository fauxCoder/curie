#pragma once

#include <cmath>

template <typename R>
struct SH
{
    SH(uint32_t a_t, uint32_t a_l, uint32_t a_shift = 0, double a_d = 0.0)
    : t(a_t)
    , l(a_l)
    , shift(a_shift)
    , d(a_d)
    , max((double)std::numeric_limits<R>::max() * 0.99)
    {
    }

    SH Sin(double f)
    {
        return SH<R>(t, l, shift, d + (std::sin((double)(t + shift) / f) * max));
    }

    SH Scale(double s)
    {
        return SH<R>(t, l, shift, d * s);
    }

    SH Cut(double c)
    {
        double level = c * max;

        double out = d;
        if (out > level)
        {
            out = level;
        }
        else if (d < -level)
        {
            out = -level;
        }

        return SH<R>(t, l, shift, out);
    }

    SH Shift(uint32_t s)
    {
        return SH<R>(t, l, shift + s, d);
    }

    R Done()
    {
        return (R)d;
    }

    uint32_t t;
    uint32_t l;
    uint32_t shift;
    double d;
    double max;
};
