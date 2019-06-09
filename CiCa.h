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

        uint32_t Key;
        int32_t X;
        int32_t Y;
        bool Set;
    };

    CiCa(int64_t a_Priority)
    : m_W(&m_Ts[0])
    , m_R(&m_Ts[1])
    , m_Priority(a_Priority)
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
    int64_t m_Priority;
};
