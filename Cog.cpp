#include <Curie/Cog.h>
#include <Curie/Quartz.h>

Cog::Cog(Quartz& a_Q,
    std::function<void(void)> a_fit_t,
    std::function<void(void)> a_fit_q,
    std::function<void(void)> a_drive_t,
    std::function<void(void)> a_drive_q)
: m_Q(a_Q)
, m_fit_t(a_fit_t)
, m_fit_q(a_fit_q)
, m_drive_t(a_drive_t)
, m_drive_q(a_drive_q)
, m_fit(0)
, m_engaged(false)
{
    std::unique_lock<std::mutex> lk(m_Q.m_cogs_mutex);
    m_Q.m_cogs.insert(this);
}

Cog::~Cog()
{
    std::unique_lock<std::mutex> lk(m_Q.m_cogs_mutex);
    m_Q.m_cogs.erase(this);
}
