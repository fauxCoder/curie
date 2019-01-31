#pragma once

#include <functional>
#include <mutex>

struct Quartz;

struct Cog
{
    Cog(Quartz& a_Q, std::function<void(void)> a_Function);

    ~Cog();

    void Move();

    std::mutex& GetMutex();

    Quartz& m_Q;
    std::function<void(void)> m_Function;
};
