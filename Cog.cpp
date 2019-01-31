#include <Curie/Cog.h>
#include <Curie/Quartz.h>

Cog::Cog(Quartz& a_Q, std::function<void(void)> a_Function)
: m_Q(a_Q)
, m_Function(a_Function)
{
    std::unique_lock<std::mutex> lk(m_Q.m_CogsMutex);
    m_Q.m_Cogs.insert(this);
}

Cog::~Cog()
{
    std::unique_lock<std::mutex> lk(m_Q.m_CogsMutex);
    m_Q.m_Cogs.erase(this);
}

void Cog::Move()
{
    m_Function();
}

std::mutex& Cog::GetMutex()
{
    return m_Q.m_CogsMutex;
}
