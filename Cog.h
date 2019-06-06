#pragma once

#include <atomic>
#include <functional>
#include <mutex>

struct Quartz;

struct Cog
{
    Cog(Quartz& a_Q, std::function<void(void)> a_Contact, std::function<void(void)> a_Tooth);

    ~Cog();

    Quartz& m_Q;
    std::function<void(void)> m_Contact;
    std::function<void(void)> m_Tooth;
    std::atomic<bool> m_Engaged;
};
