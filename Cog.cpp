#include <Curie/Cog.h>
#include <Curie/Quartz.h>

Cog::Cog(Quartz& a_Q, std::function<void(void)> a_Contact, std::function<void(void)> a_Move)
: m_Q(a_Q)
, m_Contact(a_Contact)
, m_Move(a_Move)
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
