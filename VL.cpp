#include <Curie/VL.h>

#include <cassert>

namespace VL
{

System::System()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Error: %s\n", SDL_GetError());
    }

    IMG_Init(IMG_INIT_PNG);
}

System::~System()
{
    SDL_Quit();
}

Window::Window()
{
    uint32_t s_ScreenWidth = 640;
    uint32_t s_ScreenHeight = 480;

    window = SDL_CreateWindow("Curie", 0, 0, s_ScreenWidth, s_ScreenHeight, SDL_WINDOW_SHOWN);
    assert(window);

    SDL_ShowCursor(SDL_DISABLE);
}

Window::~Window()
{
    SDL_DestroyWindow(window);
}

}
