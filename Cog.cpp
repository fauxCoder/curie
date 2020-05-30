#include <Curie/Cog.h>
#include <Curie/Quartz.h>

Cog::Cog(Quartz& a_Q,
    std::function<void(void)> a_fit_father,
    std::function<void(void)> a_drive_father,
    std::function<void(void)> a_fit_quartz,
    std::function<void(void)> a_drive_quartz)
: m_Q(a_Q)
, m_fit_father(a_fit_father)
, m_drive_father(a_drive_father)
, m_fit_quartz(a_fit_quartz)
, m_drive_quartz(a_drive_quartz)
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
