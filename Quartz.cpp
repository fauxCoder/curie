#include <Curie/Cog.h>
#include <Curie/Quartz.h>
#include <Curie/Flick.h>

#include <chrono>

const uint32_t Quartz::s_FPS = 25;
const uint32_t Quartz::s_Frequency = 1000 / s_FPS;

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
        // uint32_t start = SDL_GetTicks();
        // auto start = std::chrono::steady_clock::now();

        {
            std::unique_lock<std::mutex> lk(m_Mutex);

            if (m_ToothTokens)
                m_ToothTokens = 0;
        }

        m_Monitor.notify_one();

        {
            bool power = true;
            std::unique_lock<std::mutex> lk(m_Mutex);

            m_Monitor.wait_for(lk, std::chrono::milliseconds(s_Frequency), [&]{
                return (m_ToothTokens > 0) || ( ! (power = m_Power.load()));
            });

            if ( ! power)
                break;
        }

        {
            std::unique_lock<std::mutex> lk(m_CogsMutex);

            for (auto c : m_Cogs)
            {
                if (c->alive())
                    c->m_Move();
            }
        }

        // uint32_t elapsed = SDL_GetTicks() - start;
        // auto end = std::chrono::steady_clock::now();
        // std::chrono::duration<double, std::milli> elapsed = end - start;
        // if (((uint32_t)elapsed.count()) < s_Frequency)
        {
            // SDL_Delay(s_Frequency - elapsed);
            std::this_thread::sleep_for(std::chrono::milliseconds{40});
        }
    }
}

void Quartz::Tooth()
{
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Monitor.wait_for(lk, std::chrono::milliseconds(s_Frequency), [&]{
            return m_ToothTokens == 0;
        });

        {
            std::unique_lock<std::mutex> lk(m_CogsMutex);

            for (auto c : m_Cogs)
            {
                if (c->alive())
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
