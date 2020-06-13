#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <set>

namespace Curie
{

struct Cog;

struct Quartz
{
    friend Cog;

    Quartz(uint32_t a_FPS);

    ~Quartz();

    void resonate();

    void tooth();

    void teeth(uint32_t);

    void wait(uint32_t a_ms) { teeth(a_ms / m_FrameLength.count()); }

public:
    const uint32_t m_FPS;
    const std::chrono::milliseconds m_FrameLength;

private:
    std::atomic<bool> m_Power;

    uint32_t m_ToothTokens;
    std::mutex m_Mutex;
    std::condition_variable m_Monitor;

    std::mutex m_cogs_mutex;
    std::set<Cog*> m_cogs;

    std::thread m_thread;
};

}
