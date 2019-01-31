#pragma once

#include <Curie/Cog.h>

#include <SDL2/SDL.h>

#include <assert.h>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

struct Quartz;

struct Image
{
    SDL_Texture* Texture;
    int32_t W;
    int32_t H;
};

struct End
{
    End(int32_t a_P, void* a_Owner, std::function<void(SDL_Rect&)> a_F)
    : P(a_P)
    , Owner(a_Owner)
    , F(a_F)
    {
    }

    bool operator<(const End& rhs) const
    {
        if (P < rhs.P)
        {
            return true;
        }
        else
        {
            return Owner < rhs.Owner;
        }
    }

    int32_t P;
    void* Owner;
    std::function<void(SDL_Rect&)> F;
};

struct RM
{
    static SDL_Window* Create();

    static void Destroy(SDL_Window* a_Window);

    template <typename S, typename T>
    static uint32_t MakeKey(S a_Spice, std::vector<T> a_Values)
    {
        uint32_t ret = 0;
        S* p1 = reinterpret_cast<S*>(&ret);

        memcpy(p1++, &a_Spice, sizeof(S));

        T* p2 = reinterpret_cast<T*>(p1);

        uint32_t i = 0;
        for (auto& v : a_Values)
        {
            assert((sizeof(S) + (sizeof(T) * i)) <= sizeof(ret));

            memcpy(p2++, &v, sizeof(T));
            i++;
        }

        return ret;
    }

    RM(Quartz& a_Q, SDL_Window& a_Window);
    ~RM();

    void AddImage(uint32_t a_Key, std::string a_Image);

    Image* GetImage(uint32_t a_Key);

    End* AddEnd(std::function<void(SDL_Rect&)> a_F, void* a_Owner, int32_t a_P = 0);

    void RemoveEnd(End* a_End);

    void See();

    void Copy(uint32_t a_Key, SDL_Rect& a_Rect);

    void Copy(Image& a_Image, SDL_Rect& a_Rect);

    SDL_Window& m_Window;
    Cog m_Cog;
    SDL_Renderer* m_Renderer;

    std::mutex m_Mutex;
    std::map<uint32_t, Image> m_Images;
    std::set<End> m_Purview;
};
