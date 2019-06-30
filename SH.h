#pragma once

#include <cmath>
#include <cstdint>

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
        return SH(t, l, d + (std::sin((double)(t / f))));
    }

    SH Saw(double f)
    {
        return SH(t, l, d + (t * f));
    }

    SH Scale(double s)
    {
        return SH(t, l, d * s);
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

        return SH(t, l, out);
    }

    double Done()
    {
        return d;
    }

    uint32_t t;
    uint32_t l;
    double d;
};
