#include <Curie/Quartz.h>
#include <Curie/RM.h>

#include <SDL2/SDL_image.h>

#include <cassert>
#include <iostream>

uint32_t RM::s_ScreenWidth = 640;
uint32_t RM::s_ScreenHeight = 480;

SDL_Window* RM::CreateWindow()
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
, m_Redraw(false)
, m_Cog(a_Q, std::bind(&RM::Switch, this), std::bind(&RM::See, this))
{
    IMG_Init(IMG_INIT_PNG);

    m_Cog.m_Engaged.store(true);
}

RM::~RM()
{
    m_Cog.m_Engaged.store(false);
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

CiCa::End** RM::Add()
{
    std::unique_lock<std::mutex> lk(m_BuffersMutex);

    auto cc = new CiCa();
    auto ret = m_Buffers.emplace(&cc->m_W, cc);
    assert(ret.second);
    return &cc->m_W;
}

void RM::Remove(CiCa::End** a_End)
{
    std::unique_lock<std::mutex> lk(m_BuffersMutex);

    auto it = m_Buffers.find(a_End);
    assert(it != m_Buffers.end());
    auto cc = it->second;
    m_Buffers.erase(it);
    delete cc;
}

void RM::Switch()
{
    bool switched = false;

    {
        std::unique_lock<std::mutex> lk(m_BuffersMutex);

        for (auto& b : m_Buffers)
        {
            switched = b.second->pivot() || switched;
        }
    }

    m_Redraw.store(switched);
}

void RM::See()
{
    // if ( ! m_Draw.load())
    //    return;

    SDL_SetRenderDrawColor(m_Renderer, 0x22, 0x22, 0x22, 0xFF);
    SDL_RenderClear(m_Renderer);

    {
        std::unique_lock<std::mutex> lk(m_BuffersMutex);

        SDL_Rect r;
        CiCa::End* e;
        for (auto& b : m_Buffers)
        {
            e = b.second->m_R;
            if (e->Set)
            {
                r.x = e->X;
                r.y = e->Y;
                r.w = 1;
                r.h = 1;
                Copy(e->Key, r);
            }
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
    a_Rect.w *= a_Image->W;
    a_Rect.h *= a_Image->H;
    SDL_RenderCopy(m_Renderer, (a_Image->Texture), nullptr, &a_Rect);
}
