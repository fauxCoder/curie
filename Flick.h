#pragma once

#include <SDL2/SDL.h>

struct Flick
{
    Flick(){}

    ~Flick(){}

    void write(uint32_t a_Key, SDL_Rect a_Rect)
    {
        m_Key = a_Key;
        m_Rect = a_Rect;
    }

    uint32_t read_key()
    {
        return m_Key;
    }

    SDL_Rect& read_rect()
    {
        return m_Rect;
    }

    uint32_t m_Key;
    SDL_Rect m_Rect;
};
