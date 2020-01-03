#pragma once

#include <Curie/SB.h>

#include <cassert>
#include <cmath>
#include <cstdint>

#include <iostream>

struct Wave
{
    static const uint32_t frame_fraction = 1024;

    uint32_t progress;
    uint32_t cycle;
    uint32_t pending_cycle;

    Wave()
    : progress(0)
    , cycle(frame_fraction)
    , pending_cycle(frame_fraction)
    {
    }

    void tune(double freq)
    {
        pending_cycle = std::max(1.0, SB::s_rate / freq * static_cast<double>(frame_fraction));
    }

    double operator()(uint32_t t)
    {
        static const double two_pi = std::acos(-1) * 2;

        double ret = 0;

        if (cycle > frame_fraction * 2)
        {
            ret = std::sin(two_pi * static_cast<double>(progress) / static_cast<double>(cycle));
        }

        progress += frame_fraction;

        if (progress > cycle)
        {
            progress -= cycle;
        }

        if (cycle != pending_cycle)
        {
            progress = static_cast<double>(progress) / static_cast<double>(cycle) * static_cast<double>(pending_cycle);
            cycle = pending_cycle;
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
