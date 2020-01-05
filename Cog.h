#pragma once

#include <atomic>
#include <functional>
#include <mutex>

struct Quartz;

struct Cog
{
    Cog(Quartz& a_Q,
        std::function<void(void)>,
        std::function<void(void)>,
        std::function<void(void)>,
        std::function<void(void)>);

    ~Cog();

    Quartz& m_Q;

    std::function<void(void)> m_fit_t;
    std::function<void(void)> m_fit_q;
    std::function<void(void)> m_drive_t;
    std::function<void(void)> m_drive_q;

    uint32_t m_fit;
    std::atomic<bool> m_engaged;
};
