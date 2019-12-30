#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>

struct Wave
{
    uint32_t offset = 0;
    double divisor;
    double pending_divisor;

    double last = 0.0;

    Wave(double a_divisor = 1.0)
    : divisor(a_divisor)
    , pending_divisor(a_divisor)
    {
    }

    double operator()(uint32_t t)
    {
        double ret = std::sin(static_cast<double>(t - offset) / divisor);

        if (pending_divisor != divisor)
        {
            if (last == 0.0 || (ret > 0.0 && last < 0.0) || (ret < 0.0 && last > 0.0))
            {
                offset = t;
                divisor = pending_divisor;

                ret = std::sin(static_cast<double>(t - offset) / divisor);
            }
        }

        return ret;
    }
};

struct SH
{
    const uint32_t t;
    const uint32_t len;
    double data;

    SH(uint32_t a_t, uint32_t a_len, double a_data = 0.0)
    : t(a_t)
    , len(a_len)
    , data(a_data)
    {
    }

    SH& wave(Wave& w)
    {
        data += w(t);

        return *this;
    }

    SH& scale(double s)
    {
        data *= s;

        return *this;
    }

    SH& cut(double c)
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

    SH& envelope(uint32_t atk, uint32_t dcy, double sst, uint32_t rel)
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

    double operator()()
    {
        return data;
    }
};
