#pragma once

#include <SDL2/SDL.h>

#include <functional>
#include <map>
#include <set>

struct Quartz;

struct Catch
{
    Catch(std::set<SDL_Keycode> a_codes, void* a_p = nullptr)
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
    std::set<SDL_Keycode> codes;
};

struct Input
{
    Input(Quartz& a_Q) : m_Q(a_Q) {}

    void Enter(std::function<void(void)> a_DefaultResponse);

    Quartz& m_Q;
    std::map<Catch, std::function<bool(SDL_Keycode)>> m_KeyDownResponses;
    std::map<Catch, std::function<bool(SDL_Keycode)>> m_KeyUpResponses;
};
