#pragma once

#include <SDL2/SDL.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <set>

struct Cog;

struct Quartz
{
    friend Cog;

    Quartz(uint32_t a_FPS);

    ~Quartz();

    void Resonate();

    void Tooth();

    void Teeth(uint32_t a_Moves);

    void Wait(uint32_t a_ms) { Teeth(a_ms / m_FrameLength.count()); }

public:
    const uint32_t m_FPS;
    const std::chrono::milliseconds m_FrameLength;

private:
    std::atomic<bool> m_Power;

    uint32_t m_ToothTokens;
    std::mutex m_Mutex;
    std::condition_variable m_Monitor;

    std::mutex m_CogsMutex;
    std::set<Cog*> m_Cogs;

    std::thread m_Thread;
};
