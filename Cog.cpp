#include <Curie/Cog.h>
#include <Curie/Quartz.h>

Cog::Cog(Quartz& a_Q, std::function<void(void)> a_Lobe, std::function<void(void)> a_Tooth)
: m_Q(a_Q)
, m_Lobe(a_Lobe)
, m_Tooth(a_Tooth)
, m_Engaged(false)
{
    std::unique_lock<std::mutex> lk(m_Q.m_CogsMutex);
    m_Q.m_Cogs.insert(this);
}

Cog::~Cog()
{
    std::unique_lock<std::mutex> lk(m_Q.m_CogsMutex);
    m_Q.m_Cogs.erase(this);
}

std::mutex& Cog::GetMutex()
{
    return m_Q.m_CogsMutex;
}
