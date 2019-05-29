#include <Curie/Quartz.h>
#include <Curie/RM.h>

#include <SDL2/SDL_image.h>

#include <cassert>

uint32_t RM::s_ScreenWidth = 640;
uint32_t RM::s_ScreenHeight = 480;

SDL_Window* RM::Create()
{
    SDL_Window* window = SDL_CreateWindow("Curie", 0, 0, s_ScreenWidth, s_ScreenHeight, SDL_WINDOW_SHOWN /*SDL_WINDOW_FULLSCREEN*/);
    assert(window);

    SDL_ShowCursor(SDL_DISABLE);

    return window;
}

void RM::Destroy(SDL_Window* a_Window)
{
    SDL_DestroyWindow(a_Window);
}

RM::RM(Quartz& a_Q, SDL_Window& a_Window)
: m_Q(a_Q)
, m_Window(a_Window)
, m_Renderer(SDL_CreateRenderer(&m_Window, -1, SDL_RENDERER_ACCELERATED))
, m_Draw(false)
, m_Cog(a_Q, std::bind(&RM::Switch, this), std::bind(&RM::See, this))
{
    IMG_Init(IMG_INIT_PNG);
}

RM::~RM()
{
    m_Cog.m_Dead = true;
}

uint32_t RM::AddImage(std::string a_Image)
{
    SDL_Surface* loadedSurface = IMG_Load(a_Image.c_str());
    if (loadedSurface == nullptr)
    {
        printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
        return 0;
    }
    else
    {
        m_Images.emplace_back(new Image(SDL_CreateTextureFromSurface(m_Renderer, loadedSurface), loadedSurface->w, loadedSurface->h));
        return m_Images.size() - 1;
    }
}

Image* RM::GetImage(uint32_t a_Key)
{
    if (a_Key < m_Images.size())
    {
        return m_Images[a_Key].get();
    }
    else
    {
        return nullptr;
    }
}

Flick* RM::AddFlick()
{
    auto ret = m_Flicks.insert(new Flick());
    assert(ret.second);
    return *(ret.first);
}

void RM::RemoveFlick(Flick* a_Flick)
{
    m_Flicks.erase(a_Flick);
    delete a_Flick;
}

void RM::Switch()
{
    m_Draw = false;

    for (auto f : m_Flicks)
    {
        m_Draw = f->Switch() || m_Draw;
    }
}

void RM::See()
{
    // if ( ! m_Draw)
    //     return;

    SDL_SetRenderDrawColor(m_Renderer, 0x22, 0x22, 0x22, 0xFF);
    SDL_RenderClear(m_Renderer);

    for (auto f : m_Flicks)
    {
        if (f->yes())
        {
            Copy(f->read_key(), f->read_rect());
        }
    }

    SDL_RenderPresent(m_Renderer);
}

void RM::Copy(uint32_t a_Key, SDL_Rect& a_Rect)
{
    auto i = GetImage(a_Key);
    if (i)
    {
        Copy(i, a_Rect);
    }
}

void RM::Copy(Image* a_Image, SDL_Rect& a_Rect)
{
    a_Rect.w = a_Image->W;
    a_Rect.h = a_Image->H;
    SDL_RenderCopy(m_Renderer, (a_Image->Texture), nullptr, &a_Rect);
}
