#pragma once

#include <Curie/Cog.h>
#include <Curie/CiCa.h>
#include <Curie/VL.h>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>

struct Quartz;

namespace Curie
{
namespace Vis
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

struct RM
{
    static uint32_t s_ScreenWidth;
    static uint32_t s_ScreenHeight;

    RM(Quartz&, VL::Window&);

    ~RM();

    void init_flip();

    void flip();

    void init_draw();

    void draw();

    uint32_t AddImage(std::string);

    VL::Image* GetImage(uint32_t a_Key);

    Entry Add(int64_t a_Priority = 0);

    void Remove(Entry a_Entry);

    void Copy(uint32_t a_key, VL::Rect&);

    void Copy(VL::Image&, VL::Rect&);

    Quartz& m_Q;
    VL::Window& m_window;

    std::vector<std::unique_ptr<VL::Image>> m_Images;

    std::mutex m_Mutex;
    std::set<Entry> m_Entries;

    std::atomic<bool> m_Redraw;

    Cog m_Cog;
};

}
}
