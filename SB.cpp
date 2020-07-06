#include <Curie/SB.h>

#include <Curie/Quartz.h>

#include <algorithm>
#include <cassert>
#include <chrono>

namespace Curie
{
namespace Snd
{

working_t SB::as_working(SL::output_t a_S)
{
    return static_cast<working_t>(a_S) / static_cast<working_t>(std::numeric_limits<SL::output_t>::max());
}

SL::output_t SB::as_output(working_t a_S)
{
    return static_cast<SL::output_t>(a_S * static_cast<working_t>(std::numeric_limits<SL::output_t>::max()));
}

void SB::combine(working_t& o_A, working_t a_B)
{
    static working_t correction = std::abs(o_A) * std::abs(a_B);
    if (o_A > 0.0 && a_B > 0.0)
    {
        o_A += a_B;
        o_A -= correction;
    }
    else if (o_A < 0.0 && a_B < 0.0)
    {
        o_A += a_B;
        o_A += correction;
    }
    else
    {
        o_A += a_B;
    }
}

SB::SB(Quartz& a_Q, uint32_t a_channels_out)
: m_Q(a_Q)
, m_channels_out(a_channels_out)
, m_Start(1)
, m_Power(true)
, m_Thread(&SB::Mix, this)
{
}

SB::~SB()
{
    m_Power.store(false);

    m_Thread.join();
}

uint32_t SB::SForF(double a_Frames)
{
    return a_Frames * (s_rate / (double)m_Q.m_FPS);
}

std::shared_ptr<Sound> SB::CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&)> a_func)
{
    auto sound = std::make_shared<Sound>(1);

    uint32_t so_far = 0;
    while (so_far < a_samples)
    {
        auto& data = sound->extend(s_chunk);

        for (uint32_t t = 0; t < s_chunk; ++t)
        {
            if (so_far < a_samples)
            {
                a_func(so_far, a_samples, data[0][t]);
                ++so_far;
            }
        }
    }

    return sound;
}

std::shared_ptr<Sound> SB::CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&, working_t&)> a_func)
{
    auto sound = std::make_shared<Sound>(2);

    uint32_t so_far = 0;
    while (so_far < a_samples)
    {
        auto& data = sound->extend(s_chunk);

        for (uint32_t t = 0; t < s_chunk; ++t)
        {
            if (so_far < a_samples)
            {
                a_func(so_far, a_samples, data[0][t], data[1][t]);
                ++so_far;
            }
        }
    }

    return sound;
}

void SB::PlaySound(std::shared_ptr<Sound>& a_sound)
{
    std::unique_lock<std::mutex> lk(m_queue_mutex);

    m_queue.insert(a_sound);
}

uint32_t SB::AddSource(std::function<void(working_t*, size_t)> a_func)
{
    if (! m_Available.empty())
    {
        auto ret = m_Available.front();
        assert(m_Sources.find(ret) == m_Sources.end());
        m_Sources[ret] = a_func;
        m_Available.pop_front();
        return ret;
    }
    else if (m_Start != 0)
    {
        assert(m_Sources.find(m_Start) == m_Sources.end());
        m_Sources[m_Start] = a_func;
        return m_Start++;
    }
    else
    {
        return 0;
    }
}

void SB::RemoveSource(uint32_t a_key)
{
    if (a_key != 0)
    {
        assert(m_Sources.find(a_key) != m_Sources.end());
        m_Sources.erase(a_key);
        m_Available.push_back(a_key);
    }
}

void SB::Mix()
{
    SL::Stream stream(s_rate, m_channels_out, s_chunk);

    if (stream.channels_out() == 0)
        return;

    std::vector<working_t> work_buffer;
    work_buffer.resize(s_chunk * stream.channels_out());

    std::vector<SL::output_t> out_buffer;
    out_buffer.resize(s_chunk * stream.channels_out());

    while (m_Power.load())
    {
        std::fill(work_buffer.begin(), work_buffer.end(), 0.0);

        if (m_queue_mutex.try_lock())
        {
            for (auto key : m_queue)
            {
                m_playing.emplace_back(key);
            }

            m_queue.clear();

            m_queue_mutex.unlock();
        }

        auto it = m_playing.begin();
        while (it != m_playing.end())
        {
            const auto& sound = it->sound;
            const auto& chunk = sound->data[it->progress++];

            uint32_t channel = 0;
            uint32_t in_channel = 0;
            uint32_t out_channel = 0;
            while (true)
            {
                in_channel = channel < sound->channels ? channel : in_channel;
                out_channel = channel < stream.channels_out() ? channel : out_channel;

                for (uint32_t i = 0; i < s_chunk; ++i)
                {
                    combine(work_buffer[i * stream.channels_out() + out_channel], chunk[in_channel][i] * it->scale);
                }

                ++channel;

                if (channel >= sound->channels && channel >= stream.channels_out())
                    break;
            }

            if (it->progress == sound->data.size())
            {
                it = m_playing.erase(it);
            }
            else
            {
                ++it;
            }
        }

        for (auto& src : m_Sources)
        {
            src.second(work_buffer.data(), work_buffer.size());
        }

        for (uint32_t i = 0; i < out_buffer.size(); ++i)
        {
            out_buffer[i] = as_output(work_buffer[i]);
        }

        stream.write(out_buffer.data(), s_chunk);
    }
}

}
}
