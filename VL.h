#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>

namespace VL
{

struct System
{
    System();
    ~System();
};

using Handle = SDL_Window;

struct Window
{
    Window();
    ~Window();

    Handle* window;
};

struct Image
{
    Image(std::string file)
    : surface(nullptr)
    , texture(nullptr)
    , W(0)
    , H(0)
    {
        surface = IMG_Load(file.c_str());
        if (surface == nullptr)
        {
            printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
        }
        else
        {
            W = surface->w;
            H = surface->h;
        }
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
