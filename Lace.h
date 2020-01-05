#pragma once

#include <Curie/Quartz.h>

#include <cstdint>

struct Lace
{
    Lace(Quartz& a_Q, uint32_t a_length)
    : m_Q(a_Q)
    , m_length(a_length)
    , m_waited(0)
    {
    }

    ~Lace()
    {
        while (m_waited < m_length)
        {
            m_Q.tooth();
            ++m_waited;
        }
    }

    bool operator()(uint32_t i, uint32_t t)
    {
        if (m_waited < m_length)
        {
            uint32_t mod = (t - 1) / m_length;
            ++mod;

            if (i % mod == 0)
            {
                m_Q.tooth();
                ++m_waited;
                return true;
            }
        }

        return false;
    }

    Quartz& m_Q;

    uint32_t m_length;
    uint32_t m_waited;
};
