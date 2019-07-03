#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>

struct SH
{
    SH(uint32_t a_t, uint32_t a_len, double a_data = 0.0)
    : t(a_t)
    , len(a_len)
    , data(a_data)
    {
    }

    SH& Sin(double f)
    {
        data += std::sin((double)(t / f));

        return *this;
    }

    SH& Saw(double f)
    {
        data += t * f;

        return *this;
    }

    SH& Scale(double s)
    {
        data *= s;

        return *this;
    }

    SH& Cut(double c)
    {
        if (data > c)
        {
            data = c;
        }
        else if (data < -c)
        {
            data = -c;
        }

        return *this;
    }

    SH& Envelope(uint32_t atk, uint32_t dcy, double sst, uint32_t rel)
    {
        double scale = 1.0;

        assert(len > t);
        uint32_t togo = len - t;
        if (togo < rel)
        {
            scale *= sst;

            scale *= static_cast<double>(togo) / static_cast<double>(rel);
        }
        else if (t < atk)
        {
            scale *= static_cast<double>(t) / static_cast<double>(atk);
        }
        else if (t < (atk + dcy))
        {
            double leftover = static_cast<double>((atk + dcy) - t) / static_cast<double>(dcy);
            scale *= (sst + ((1.0 - sst) * leftover));
        }
        else
        {
            scale *= sst;
        }

        data *= scale;

        return *this;
    }

    double Done()
    {
        return data;
    }

    const uint32_t t;
    const uint32_t len;
    double data;
};
