#include <Curie/Quartz.h>
#include <Curie/RM.h>

#include <cassert>
#include <iostream>

RM::RM(Quartz& a_Q, VL::Window& a_window)
: m_Q(a_Q)
, m_window(a_window)
, m_Redraw(false)
, m_Cog(a_Q,
    std::bind(&RM::init_flip, this),
    std::bind(&RM::flip, this),
    std::bind(&RM::init_draw, this),
    std::bind(&RM::draw, this))
{
    m_Cog.m_engaged.store(true);
}

RM::~RM()
{
    m_Cog.m_engaged.store(false);
}

uint32_t RM::AddImage(std::string a_Image)
{
    m_Images.emplace_back(new VL::Image(a_Image));
    return m_Images.size() - 1;
}

VL::Image* RM::GetImage(uint32_t a_Key)
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

RM::Entry RM::Add(int64_t a_Priority)
{
    std::unique_lock<std::mutex> lk(m_Mutex);

    auto cc = new CiCa(a_Priority);
    auto ret = m_Entries.emplace(cc);
    assert(ret.second);
    return *ret.first;
}

void RM::Remove(Entry a_Entry)
{
    std::unique_lock<std::mutex> lk(m_Mutex);

    auto it = m_Entries.find(a_Entry);
    assert(it != m_Entries.end());
    auto cc = it->m_CiCa;
    m_Entries.erase(it);
    delete cc;

    m_Redraw.store(true);
}

void RM::init_flip()
{
}

void RM::flip()
{
    bool redraw = m_Redraw.load();

    {
        std::unique_lock<std::mutex> lk(m_Mutex);

        for (auto& b : m_Entries)
        {
            redraw = b.m_CiCa->pivot() || redraw;
        }
    }

    m_Redraw.store(redraw);
}

void RM::init_draw()
{
    m_window.init();

    for (auto& image : m_Images)
    {
        image->create_texture(m_window);
    }
}

void RM::draw()
{
    if (!m_Redraw.load())
        return;

    m_Redraw.store(false);

    m_window.clear();

    {
        std::unique_lock<std::mutex> lk(m_Mutex);

        VL::Rect r;
        CiCa::End* e;
        for (auto& b : m_Entries)
        {
            e = b.m_CiCa->m_R;
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

    m_window.present();
}

void RM::Copy(uint32_t a_key, VL::Rect& a_rect)
{
    auto i = GetImage(a_key);
    if (i)
    {
        Copy(*i, a_rect);
    }
}

void RM::Copy(VL::Image& a_image, VL::Rect& a_rect)
{
    a_rect.w *= a_image.w;
    a_rect.h *= a_image.h;

    m_window.copy(a_image, a_rect);
}
