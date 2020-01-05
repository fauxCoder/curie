#include <Curie/Cog.h>
#include <Curie/Quartz.h>

using namespace std::chrono;

Quartz::Quartz(uint32_t a_FPS)
: m_FPS(a_FPS)
, m_FrameLength(1000 / a_FPS)
, m_Power(true)
, m_ToothTokens(0)
, m_thread(&Quartz::resonate, this)
{
}

Quartz::~Quartz()
{
    m_Power.store(false);

    m_Monitor.notify_all();

    m_thread.join();
}

void Quartz::resonate()
{
    while (m_Power.load())
    {
        auto start = std::chrono::steady_clock::now();

        {
            std::unique_lock<std::mutex> lk(m_Mutex);

            if (m_ToothTokens)
            {
                m_ToothTokens = 0;
            }
        }

        m_Monitor.notify_one();

        {
            bool power = true;
            std::unique_lock<std::mutex> lk(m_Mutex);

            m_Monitor.wait(lk, [&]{
                return (m_ToothTokens > 0) || ( ! (power = m_Power.load()));
            });

            if ( ! power)
                break;
        }

        {
            std::unique_lock<std::mutex> lk(m_cogs_mutex);

            for (auto c : m_cogs)
            {
                if (c->m_engaged.load())
                {
                    if (c->m_fit == 1)
                    {
                        c->m_fit_q();
                        c->m_fit = 2;
                    }

                    c->m_drive_q();
                }
            }
        }

        auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start);
        if (elapsed < m_FrameLength)
        {
            std::this_thread::sleep_for(m_FrameLength - elapsed);
        }
    }
}

void Quartz::tooth()
{
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Monitor.wait(lk, [&]{
            return m_ToothTokens == 0;
        });

        {
            std::unique_lock<std::mutex> lk(m_cogs_mutex);

            for (auto c : m_cogs)
            {
                if (c->m_engaged.load())
                {
                    if (c->m_fit == 0)
                    {
                        c->m_fit_t();
                        c->m_fit = 1;
                    }

                    c->m_drive_t();
                }
            }
        }

        m_ToothTokens++;
    }

    m_Monitor.notify_one();
}

void Quartz::teeth(uint32_t a_Moves)
{
    while (a_Moves > 0)
    {
        tooth();
        --a_Moves;
    }
}
