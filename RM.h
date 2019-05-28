#pragma once

#include <Curie/Cog.h>
#include <Curie/Flick.h>

#include <SDL2/SDL.h>

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

struct Quartz;

struct Image
{
    Image(SDL_Texture* a_Texture, int32_t a_W, int32_t a_H)
        : Texture(a_Texture)
        , W(a_W)
        , H(a_H)
    {
    }

    SDL_Texture* Texture;
    int32_t W;
    int32_t H;
};

struct RM
{
    static uint32_t s_ScreenWidth;
    static uint32_t s_ScreenHeight;

    static SDL_Window* Create();

    static void Destroy(SDL_Window* a_Window);

    RM(Quartz& a_Q, SDL_Window& a_Window);

    ~RM();

    uint32_t AddImage(std::string a_Image);

    Image* GetImage(uint32_t a_Key);

    Flick* AddFlick();

    void RemoveFlick(Flick* a_Flick);

    void Switch();

    void See();

    void Copy(uint32_t a_Key, SDL_Rect& a_Rect);

    void Copy(Image* a_Image, SDL_Rect& a_Rect);

    Quartz& m_Q;
    SDL_Window& m_Window;
    SDL_Renderer* m_Renderer;

    std::mutex m_Mutex;
    std::vector<std::unique_ptr<Image>> m_Images;
    std::set<Flick*> m_Flicks;

    Cog m_Cog;
};
