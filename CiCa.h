#pragma once

#include <algorithm>
#include <cstdint>

struct CiCa
{
    struct Thing
    {
        Thing()
        : Key(0)
        , X(0)
        , Y(0)
        , Go(false)
        {
        }

        uint32_t Key;
        int32_t X;
        int32_t Y;
        bool Go;
    };

    CiCa()
    : m_W(&m_Ts[0])
    , m_R(&m_Ts[1])
    {
    }

    void write(uint32_t a_Key, int32_t a_X, int32_t a_Y)
    {
        m_W->Key = a_Key;
        m_W->X = a_X;
        m_W->Y = a_Y;
        m_W->Go = true;
    }

    bool pivot()
    {
        if (m_W->Go)
        {
            std::swap(m_W, m_R);
            m_W->Go = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    Thing m_Ts[2];
    Thing* m_W;
    Thing* m_R;
};
