#include <Curie/VL.h>

#include <SDL2/SDL.h>

#include <cassert>

namespace VL
{

system::system()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Error: %s\n", SDL_GetError());
    }
}

system::~system()
{
    SDL_Quit();
}

window::window()
{
    uint32_t s_ScreenWidth = 640;
    uint32_t s_ScreenHeight = 480;

    window_handle = SDL_CreateWindow("Curie", 0, 0, s_ScreenWidth, s_ScreenHeight, SDL_WINDOW_SHOWN);
    assert(window_handle);

    SDL_ShowCursor(SDL_DISABLE);
}

window::~window()
{
    SDL_DestroyWindow(window_handle);
}

}
