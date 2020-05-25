#pragma once

#include <SDL2/SDL.h>

namespace VL
{

struct system
{
    system();
    ~system();
};

using handle = SDL_Window;

struct window
{
    window();
    ~window();

    handle* window_handle;
};

struct Image
{
    Image(SDL_Surface* a_surface, int32_t a_W, int32_t a_H)
    : surface(a_surface)
    , texture(nullptr)
    , W(a_W)
    , H(a_H)
    {
    }

    void create_texture(SDL_Renderer* a_renderer)
    {
        texture = SDL_CreateTextureFromSurface(a_renderer, surface);
    }

    SDL_Surface* surface;
    SDL_Texture* texture;
    int32_t W;
    int32_t H;
};

}
