#include <Curie/Cog.h>
#include <Curie/Quartz.h>

Quartz::Quartz()
: m_Power(true)
, m_Thread(&Quartz::Resonate, this)
, m_Waiting(false)
, m_FrameDone(false)
{
}

Quartz::~Quartz()
{
    m_Power.store(false);
    m_Thread.join();
}

void Quartz::Resonate()
{
    while (m_Power.load())
    {
        uint32_t start = SDL_GetTicks();

        {
            std::unique_lock<std::mutex> lk(m_CogsMutex);

            for (auto c : m_Cogs)
            {
                c->Move();
            }
        }

        {
            std::unique_lock<std::mutex> lk(m_Mutex);
            m_Monitor.wait(lk, [&]{ return ( ! m_Waiting) || ( ! m_FrameDone); });
            if (m_Waiting)
            {
                m_FrameDone = true;
            }
        }

        m_Monitor.notify_one();

        uint32_t elapsed = SDL_GetTicks() - start;
        if (elapsed < s_Frequency)
        {
            SDL_Delay(s_Frequency - elapsed);
        }
    }
}

void Quartz::Wait(uint32_t a_Moves)
{
    std::unique_lock<std::mutex> lk(m_Mutex);
    m_Waiting = true;

    while (a_Moves > 0)
    {
        if ( ! lk.owns_lock()) lk.lock();
        m_Monitor.wait(lk, [&]{ return m_FrameDone; });
        m_FrameDone = false;
        lk.unlock();

        m_Monitor.notify_one();

        --a_Moves;
    }

    if ( ! lk.owns_lock()) lk.lock();
    m_Waiting = false;
    lk.unlock();

    m_Monitor.notify_one();
}
