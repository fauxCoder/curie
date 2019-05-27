#pragma once

#include <functional>
#include <mutex>

struct Quartz;

struct Cog
{
    Cog(Quartz& a_Q, std::function<void(void)> a_Contact, std::function<void(void)> a_Move);

    ~Cog();

    std::mutex& GetMutex();

    bool alive() { return ! m_Dead; }

    Quartz& m_Q;
    std::function<void(void)> m_Contact;
    std::function<void(void)> m_Move;
    bool m_Dead;
};
