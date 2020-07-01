#include <Curie/VL.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cassert>

namespace Curie
{
namespace VL
{

struct Window::impl
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

struct Image::impl
{
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;
    int32_t w = 0;
    int32_t h = 0;
};

System::System()
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Error: %s\n", SDL_GetError());
    }

    IMG_Init(IMG_INIT_PNG);
}

System::~System()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

Window::Window()
: pimpl(std::make_unique<impl>())
{
    uint32_t s_ScreenWidth = 640;
    uint32_t s_ScreenHeight = 480;

    pimpl->window = SDL_CreateWindow("Curie", 0, 0, s_ScreenWidth, s_ScreenHeight, SDL_WINDOW_SHOWN);
    assert(pimpl->window);

    SDL_ShowCursor(SDL_DISABLE);
}

Window::~Window()
{
    SDL_DestroyWindow(pimpl->window);
}

void Window::init()
{
    pimpl->renderer = SDL_CreateRenderer(pimpl->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Window::clear()
{
    SDL_SetRenderDrawColor(pimpl->renderer, 0x22, 0x22, 0x22, 0xff);
    SDL_RenderClear(pimpl->renderer);
}

void Window::copy(VL::Image& a_image, VL::Rect& a_rect)
{
    SDL_Rect* r = reinterpret_cast<SDL_Rect*>(a_rect.rect);
    SDL_RenderCopy(pimpl->renderer, (a_image.pimpl->texture), nullptr, r);
}

void Window::present()
{
    SDL_RenderPresent(pimpl->renderer);
}

Image::Image(std::string file)
: pimpl(std::make_unique<impl>())
{
    pimpl->surface = IMG_Load(file.c_str());
    if (pimpl->surface == nullptr)
    {
        printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
    }
    else
    {
        pimpl->w = pimpl->surface->w;
        pimpl->h = pimpl->surface->h;
    }
}

Image::~Image()
{
    if (pimpl->texture)
    {
        SDL_DestroyTexture(pimpl->texture);
    }
}

void Image::init(Window& a_window)
{
    if (pimpl->texture == nullptr)
    {
        pimpl->texture = SDL_CreateTextureFromSurface(a_window.pimpl->renderer, pimpl->surface);
    }
}

int32_t Image::get_width()
{
    return pimpl->w;
}

int32_t Image::get_height()
{
    return pimpl->h;
}

}
}
