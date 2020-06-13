#pragma once

#include <Curie/EL.h>

#include <functional>
#include <map>
#include <set>

namespace Curie
{

struct Catch
{
    Catch(std::set<EL::Event> a_codes, void* a_p = nullptr)
    : p (a_p)
    , codes(a_codes)
    {
    }

    bool operator<(const Catch& c) const
    {
        if (p != c.p)
        {
            return p < c.p;
        }
        else
        {
            return codes < c.codes;
        }
    }

    void* p;
    std::set<EL::Event> codes;
};

struct Input
{
    template<typename T>
    void link(T& a_t)
    {
        m_KeyDownResponses[Catch(a_t.keys_down(), &a_t)] =
            std::bind(&T::key_down, &a_t, std::placeholders::_1);

        m_KeyUpResponses[Catch(a_t.keys_up(), &a_t)] =
            std::bind(&T::key_up, &a_t, std::placeholders::_1);
    }

    void open(std::function<void(void)> a_DefaultResponse);

    std::map<Catch, std::function<bool(EL::Event)>> m_KeyDownResponses;
    std::map<Catch, std::function<bool(EL::Event)>> m_KeyUpResponses;
};

}
