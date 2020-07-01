#include <Curie/VL.h>

#include <cassert>

namespace Curie
{
namespace VL
{

struct Window::impl
{
    //SDL_Window* window = nullptr;
    //SDL_Renderer* renderer = nullptr;
};

struct Image::impl
{
    //SDL_Surface* surface = nullptr;
    //SDL_Texture* texture = nullptr;
    int32_t w = 0;
    int32_t h = 0;
};

System::System()
{
}

System::~System()
{
}

Window::Window()
: pimpl(std::make_unique<impl>())
{
    //uint32_t s_ScreenWidth = 640;
    //uint32_t s_ScreenHeight = 480;
}

Window::~Window()
{
}

void Window::init()
{
    //pimpl->renderer = SDL_CreateRenderer(pimpl->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Window::clear()
{
    //SDL_SetRenderDrawColor(pimpl->renderer, 0x22, 0x22, 0x22, 0xff);
    //SDL_RenderClear(pimpl->renderer);
}

void Window::copy(VL::Image& a_image, VL::Rect& a_rect)
{
    //SDL_Rect* r = reinterpret_cast<SDL_Rect*>(a_rect.rect);
    //SDL_RenderCopy(pimpl->renderer, (a_image.pimpl->texture), nullptr, r);
}

void Window::present()
{
    //SDL_RenderPresent(pimpl->renderer);
}

Image::Image(std::string file)
: pimpl(std::make_unique<impl>())
{
    //{
    //    pimpl->w = pimpl->surface->w;
    //    pimpl->h = pimpl->surface->h;
    //}
}

Image::~Image()
{
}

void Image::init(Window& a_window)
{
    //if (pimpl->texture == nullptr)
    //{
    //    pimpl->texture = SDL_CreateTextureFromSurface(a_window.pimpl->renderer, pimpl->surface);
    //}
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
