#include <Curie/Cog.h>
#include <Curie/Quartz.h>
#include <Curie/Flick.h>

using namespace std::chrono;

const uint32_t Quartz::s_FPS = 25;
const milliseconds Quartz::s_FrameLength = milliseconds(1000 / s_FPS);

Quartz::Quartz()
: m_Power(true)
, m_ToothTokens(0)
, m_Thread(&Quartz::Resonate, this)
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
        auto start = std::chrono::steady_clock::now();

        {
            std::unique_lock<std::mutex> lk(m_Mutex);

            if (m_ToothTokens)
                m_ToothTokens = 0;
        }

        m_Monitor.notify_one();

        {
            bool power = true;
            std::unique_lock<std::mutex> lk(m_Mutex);

            m_Monitor.wait_for(lk, s_FrameLength, [&]{
                return (m_ToothTokens > 0) || ( ! (power = m_Power.load()));
            });

            if ( ! power)
                break;
        }

        {
            std::unique_lock<std::mutex> lk(m_CogsMutex);

            for (auto c : m_Cogs)
            {
                if (c->m_Engaged.load())
                    c->m_Move();
            }
        }

        auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start);
        if (elapsed < s_FrameLength)
        {
            std::this_thread::sleep_for(s_FrameLength - elapsed);
        }
    }
}

void Quartz::Tooth()
{
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Monitor.wait_for(lk, s_FrameLength, [&]{
            return m_ToothTokens == 0;
        });

        {
            std::unique_lock<std::mutex> lk(m_CogsMutex);

            for (auto c : m_Cogs)
            {
                if (c->m_Engaged.load())
                    c->m_Contact();
            }
        }

        m_ToothTokens++;
    }

    m_Monitor.notify_one();
}

void Quartz::Wait(uint32_t a_Moves)
{
    while (a_Moves > 0)
    {
        Tooth();
        --a_Moves;
    }
}
