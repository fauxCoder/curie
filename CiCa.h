#pragma once

#include <algorithm>
#include <cstdint>

struct CiCa
{
    struct End
    {
        End()
        : Key(0)
        , X(0)
        , Y(0)
        , Set(false)
        {
        }

        void write(uint32_t a_Key, int32_t a_X, int32_t a_Y)
        {
            Key = a_Key;
            X = a_X;
            Y = a_Y;
            Set = true;
        }

        uint32_t Key;
        int32_t X;
        int32_t Y;
        bool Set;
    };

    CiCa()
    : m_W(&m_Ts[0])
    , m_R(&m_Ts[1])
    {
    }

    bool pivot()
    {
        if (m_W->Set)
        {
            std::swap(m_W, m_R);
            m_W->Set = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    End m_Ts[2];
    End* m_W;
    End* m_R;
};