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

struct Image;

struct Rect
{
    union
    {
        SDL_Rect _rect;
        struct
        {
            int x, y, w, h;
        };
    };
};

struct Window
{
    Window();
    ~Window();

    void init();

    void clear();

    void copy(Image&, Rect&);

    void present();

    SDL_Window* _window;
    SDL_Renderer* _renderer;
};

struct Image
{
    Image(std::string);

    ~Image();

    void create_texture(Window&);

    SDL_Surface* _surface;
    SDL_Texture* _texture;
    int32_t w;
    int32_t h;
};

}
