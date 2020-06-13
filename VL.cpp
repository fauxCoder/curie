#include <Curie/VL.h>

#include <cassert>

namespace Curie
{
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
: _window(nullptr)
, _renderer(nullptr)
{
    uint32_t s_ScreenWidth = 640;
    uint32_t s_ScreenHeight = 480;

    _window = SDL_CreateWindow("Curie", 0, 0, s_ScreenWidth, s_ScreenHeight, SDL_WINDOW_SHOWN);
    assert(_window);

    SDL_ShowCursor(SDL_DISABLE);
}

Window::~Window()
{
    SDL_DestroyWindow(_window);
}

void Window::init()
{
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Window::clear()
{
    SDL_SetRenderDrawColor(_renderer, 0x22, 0x22, 0x22, 0xFF);
    SDL_RenderClear(_renderer);
}

void Window::copy(VL::Image& a_image, VL::Rect& a_rect)
{
    SDL_RenderCopy(_renderer, (a_image._texture), nullptr, &a_rect._rect);
}

void Window::present()
{
    SDL_RenderPresent(_renderer);
}

Image::Image(std::string file)
: _surface(nullptr)
, _texture(nullptr)
, w(0)
, h(0)
{
    _surface = IMG_Load(file.c_str());
    if (_surface == nullptr)
    {
        printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
    }
    else
    {
        w = _surface->w;
        h = _surface->h;
    }
}

Image::~Image()
{
    if (_texture)
    {
        SDL_DestroyTexture(_texture);
    }
}

void Image::create_texture(Window& a_window)
{
    if (_texture == nullptr)
    {
        _texture = SDL_CreateTextureFromSurface(a_window._renderer, _surface);
    }
}

}
}
