#pragma once

#include <SDL2/SDL.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <set>

struct Cog;

struct Quartz
{
    friend Cog;

    static const uint32_t s_FPS;

    static const uint32_t s_Frequency;

    Quartz();

    ~Quartz();

    void Resonate();

    void Tooth();

    void Wait(uint32_t a_Moves);

    void Waitms(uint32_t a_ms) { Wait(a_ms / s_Frequency); }

private:
    std::atomic<bool> m_Power;
    std::thread m_Thread;

    uint32_t m_ToothTokens;
    std::mutex m_Mutex;
    std::condition_variable m_Monitor;

    std::set<Cog*> m_Cogs;
    std::mutex m_CogsMutex;
};
