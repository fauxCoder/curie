#pragma once

#include <cmath>

struct SH
{
    SH(uint32_t a_t, uint32_t a_l, uint32_t a_shift = 0, double a_d = 0.0)
    : t(a_t)
    , l(a_l)
    , shift(a_shift)
    , d(a_d)
    {
    }

    SH Sin(double f)
    {
        return SH(t, l, shift, d + (std::sin((double)(t + shift) / f)));
    }

    SH Scale(double s)
    {
        return SH(t, l, shift, d * s);
    }

    SH Cut(double c)
    {
        double level = c;

        double out = d;
        if (out > level)
        {
            out = level;
        }
        else if (d < -level)
        {
            out = -level;
        }

        return SH(t, l, shift, out);
    }

    SH Shift(uint32_t s)
    {
        return SH(t, l, shift + s, d);
    }

    double Done()
    {
        return d;
    }

    uint32_t t;
    uint32_t l;
    uint32_t shift;
    double d;
};
