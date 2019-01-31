#include <Curie/RM.h>

#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* RM::Create()
{
    SDL_Window* window = SDL_CreateWindow("Curie", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN /*SDL_WINDOW_FULLSCREEN*/);
    assert(window);

    SDL_ShowCursor(SDL_DISABLE);

    return window;
}

void RM::Destroy(SDL_Window* a_Window)
{
    SDL_DestroyWindow(a_Window);
}

RM::RM(Quartz& a_Q, SDL_Window& a_Window)
: m_Window(a_Window)
, m_Cog(a_Q, std::bind(&RM::See, this))
, m_Renderer(nullptr)
{
    m_Renderer = SDL_CreateRenderer(&m_Window, -1, SDL_RENDERER_ACCELERATED);

    IMG_Init(IMG_INIT_PNG);
}

RM::~RM()
{
}

void RM::AddImage(uint32_t a_Key, std::string a_Image)
{
    SDL_Surface* loadedSurface = IMG_Load(a_Image.c_str());
    if (loadedSurface == nullptr)
    {
        printf("Unable to load image! SDL_image Error: %s\n", IMG_GetError());
    }
    else
    {
        m_Images[a_Key] = { SDL_CreateTextureFromSurface(m_Renderer, loadedSurface), loadedSurface->w, loadedSurface->h };
    }
}

Image* RM::GetImage(uint32_t a_Key)
{
    auto it = m_Images.find(a_Key);
    if (it != m_Images.end())
    {
        return &(it->second);
    }
    else
    {
        return nullptr;
    }
}

End* RM::AddEnd(std::function<void(SDL_Rect&)> a_F, void* a_Owner, int32_t a_P)
{
    End* e = new End(a_P, a_Owner, a_F);
    m_Purview.insert(*e);
    return e;
}

void RM::RemoveEnd(End* a_End)
{
    m_Purview.erase(*a_End);
    delete a_End;
}

void RM::See()
{
    SDL_SetRenderDrawColor(m_Renderer, 0x22, 0x22, 0x22, 0xFF);
    SDL_RenderClear(m_Renderer);

    {
        std::unique_lock<std::mutex> lk(m_Mutex);

        SDL_Rect r;
        for (auto& e : m_Purview)
        {
            e.F(r);
        }
    }

    SDL_RenderPresent(m_Renderer);
}

void RM::Copy(uint32_t a_Key, SDL_Rect& a_Rect)
{
    auto t = m_Images.find(a_Key);
    if (t != m_Images.end())
    {
        Copy(t->second, a_Rect);
    }
}

void RM::Copy(Image& a_Image, SDL_Rect& a_Rect)
{
    a_Rect.w *= a_Image.W;
    a_Rect.h *= a_Image.H;
    SDL_RenderCopy(m_Renderer, (a_Image.Texture), nullptr, &a_Rect);
}
