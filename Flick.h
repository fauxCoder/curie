#pragma once

#include <SDL2/SDL.h>

struct Flick
{
    Flick()
    : m_I(0)
    {
        m_Yes[0] = false;
        m_Yes[1] = false;
    }


    ~Flick(){}

    void Switch()
    {
        if (m_Yes[m_I])
        {
            m_I ^= 0x1;
            m_Yes[m_I] = false;
        }
    }

    void write(uint32_t a_Key, SDL_Rect a_Rect)
    {
        m_Yes[m_I] = true;
        m_Key[m_I] = a_Key;
        m_Rect[m_I] = a_Rect;
    }

    bool yes()
    {
        return m_Yes[m_I ^ 0x1];
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

    bool m_Yes[2];
    uint32_t m_Key[2];
    SDL_Rect m_Rect[2];
};
