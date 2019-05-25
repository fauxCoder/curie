#pragma once

#include <SDL2/SDL.h>

struct Flick
{
    Flick()
    : m_I(0)
    {}

    ~Flick(){}

    void Switch()
    {
        m_I ^= 0x1;
    }

    void write(uint32_t a_Key, SDL_Rect a_Rect)
    {
        m_Key[m_I] = a_Key;
        m_Rect[m_I] = a_Rect;
    }

    uint32_t read_key()
    {
        return m_Key[m_I ^ 0x1];
    }

    SDL_Rect& read_rect()
    {
        return m_Rect[m_I ^ 0x1];
    }

    uint8_t m_I;

    uint32_t m_Key[2];
    SDL_Rect m_Rect[2];
};
