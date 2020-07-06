#include <Curie/VL.h>

#include <curses.h>

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
    ::clear();
}

void Window::clear()
{
}

void Window::copy(VL::Image& a_image, VL::Rect& a_rect)
{
    // mvaddch(5, 5, '*');
}

void Window::present()
{
    refresh();
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
