#pragma once

#include <SDL2/SDL.h>

#include <functional>
#include <map>
#include <set>

struct Quartz;

struct Input
{
    Input(Quartz& a_Q) : m_Q(a_Q) {}

    void Enter(std::function<void(void)> a_DefaultResponse);

    Quartz& m_Q;
    std::map<std::set<SDL_Keycode>, std::function<bool(SDL_Keycode)>> m_KeyDownResponses;
};
