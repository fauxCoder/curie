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
: window(nullptr)
, renderer(nullptr)
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

void Window::init()
{
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Window::clear()
{
    SDL_SetRenderDrawColor(renderer, 0x22, 0x22, 0x22, 0xFF);
    SDL_RenderClear(renderer);
}

void Window::copy(VL::Image& a_image, VL::Rect& a_rect)
{
    SDL_RenderCopy(renderer, (a_image.texture), nullptr, &a_rect._rect);
}

void Window::present()
{
    SDL_RenderPresent(renderer);
}

Image::Image(std::string file)
: surface(nullptr)
, texture(nullptr)
, w(0)
, h(0)
{
    surface = IMG_Load(file.c_str());
    if (surface == nullptr)
    {
        printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
    }
    else
    {
        w = surface->w;
        h = surface->h;
    }
}

Image::~Image()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
}

void Image::create_texture(Window& window)
{
    if (texture == nullptr)
    {
        texture = SDL_CreateTextureFromSurface(window.renderer, surface);
    }
}

}
