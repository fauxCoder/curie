#pragma once

#include <Curie/Cog.h>
#include <Curie/CiCa.h>

#include <SDL2/SDL.h>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

struct Quartz;

struct Image
{
    Image(SDL_Surface* a_surface, int32_t a_W, int32_t a_H)
    : surface(a_surface)
    , texture(nullptr)
    , W(a_W)
    , H(a_H)
    {
    }

    void create_texture(SDL_Renderer* a_renderer)
    {
        texture = SDL_CreateTextureFromSurface(a_renderer, surface);
    }

    SDL_Surface* surface;
    SDL_Texture* texture;
    int32_t W;
    int32_t H;
};

struct RM
{
    struct Entry
    {
        Entry(CiCa* a_CiCa)
        : m_CiCa(a_CiCa)
        {
        }

        bool operator<(const Entry &e) const
        {
            if (m_CiCa->m_Priority == e.m_CiCa->m_Priority)
            {
                return m_CiCa < e.m_CiCa;
            }
            else
            {
                return m_CiCa->m_Priority < e.m_CiCa->m_Priority;
            }
        }

        void write(uint32_t a_Key, int32_t a_X, int32_t a_Y)
        {
            m_CiCa->m_W->Key = a_Key;
            m_CiCa->m_W->X = a_X;
            m_CiCa->m_W->Y = a_Y;
            m_CiCa->m_W->Set = true;
        }

        CiCa* m_CiCa;
    };

    static uint32_t s_ScreenWidth;
    static uint32_t s_ScreenHeight;

    static SDL_Window* CreateWindow();

    static void Destroy(SDL_Window* a_Window);

    RM(Quartz& a_Q, SDL_Window& a_Window);

    ~RM();

    void init_t();

    void init_q();

    void switch_t();

    void see_q();

    uint32_t AddImage(std::string a_Image);

    Image* GetImage(uint32_t a_Key);

    Entry Add(int64_t a_Priority = 0);

    void Remove(Entry a_Entry);

    void Copy(uint32_t a_Key, SDL_Rect& a_Rect);

    void Copy(Image* a_Image, SDL_Rect& a_Rect);

    Quartz& m_Q;
    SDL_Window& m_Window;
    SDL_Renderer* m_renderer;

    std::vector<std::unique_ptr<Image>> m_Images;

    std::mutex m_Mutex;
    std::set<Entry> m_Entries;

    std::atomic<bool> m_Redraw;

    Cog m_Cog;
};
