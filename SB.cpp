#include <Curie/SB.h>

#include <Curie/Quartz.h>

#include <cassert>
#include <chrono>
#include <iostream>

SDL_AudioDeviceID SB::Open(SB* a_SB, uint32_t a_channels)
{
    SDL_AudioSpec want;
    memset(&want, 0, sizeof(want));
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = a_channels;
    want.samples = 4096;
    want.callback = &Write;
    want.userdata = static_cast<void*>(a_SB);

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &want, &a_SB->m_Have, SDL_AUDIO_ALLOW_ANY_CHANGE);

    assert(want.freq == a_SB->m_Have.freq);
    assert(want.format == a_SB->m_Have.format);
    assert(want.channels == a_SB->m_Have.channels);

    SDL_PauseAudioDevice(device, 0);

    return device;
}

void SB::Write(void* a_SB, uint8_t* a_Stream, int32_t a_Length)
{
    SB* sb = static_cast<SB*>(a_SB);

    int32_t togo = a_Length;

    while (togo > 0)
    {
        std::unique_lock<std::mutex> lk(sb->m_write_mutex);
        if (sb->m_ready)
        {
            size_t s = sb->m_buffer.size() * sizeof(output_t);
            memcpy(a_Stream, sb->m_buffer.data(), s);
            sb->m_ready = false;

            a_Stream += s;
            togo -= s;
        }
        else
        {
            lk.unlock();
            std::this_thread::yield();
        }
    }

    assert(togo == 0);
}

void SB::Close(SDL_AudioDeviceID a_Device)
{
    SDL_CloseAudioDevice(a_Device);
}

SB::working_t SB::as_working(output_t a_S)
{
    return static_cast<working_t>(a_S) / static_cast<working_t>(std::numeric_limits<output_t>::max());
}

SB::output_t SB::as_output(working_t a_S)
{
    return static_cast<output_t>(a_S * static_cast<working_t>(std::numeric_limits<output_t>::max()));
}

void SB::combine(working_t& o_A, working_t a_B)
{
    static working_t correction = abs(o_A) * abs(a_B);
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
, m_Device(Open(this, a_channels_out))
, m_Start(1)
, m_buffer(s_chunk * m_Have.channels)
, m_ready(false)
, m_Power(true)
, m_Thread(&SB::Mix, this)
{
}

SB::~SB()
{
    Close(m_Device);

    m_Power.store(false);

    m_Thread.join();
}

uint32_t SB::SForF(double a_Frames)
{
    return a_Frames * ((double)m_Have.freq / (double)m_Q.m_FPS);
}

uint32_t
SB::CreateSound(uint32_t a_samples, uint32_t a_channels,
    std::function<void(uint32_t, uint32_t, std::vector<std::array<working_t, s_chunk>>&, size_t)> a_func)
{
    auto& sound = m_Sounds.emplace_back(a_channels);

    uint32_t so_far = 0;
    while (so_far < a_samples)
    {
        auto& data = sound.extend();

        for (uint32_t t = 0; t < s_chunk; ++t)
        {
            if (so_far < a_samples)
            {
                a_func(so_far, a_samples, data, t);
                ++so_far;
            }
        }
    }

    return m_Sounds.size() - 1;
}

void SB::PlaySound(uint32_t a_Key)
{
    std::unique_lock<std::mutex> lk(m_QueueMutex);

    if (m_ToQueue.size() < 3)
        m_ToQueue.emplace_back(a_Key);
}

uint8_t SB::AddSource(std::function<void(working_t*, size_t)> a_func)
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

void SB::RemoveSource(uint8_t a_key)
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
    std::vector<working_t> work_buffer;
    work_buffer.resize(s_chunk * m_Have.channels);

    while (m_Power.load())
    {
        std::unique_lock<std::mutex> wlk(m_write_mutex);

        if (!m_ready)
        {
            std::unique_lock<std::mutex> qlk(m_QueueMutex);

            std::fill(work_buffer.begin(), work_buffer.end(), 0.0);

            auto it = m_ToQueue.begin();
            while (it != m_ToQueue.end())
            {
                const auto& sound = m_Sounds[it->key];
                const auto& chunk = sound.data[it->progress++];

                uint32_t channel = 0;
                uint32_t in_channel = 0;
                uint32_t out_channel = 0;
                while (true)
                {
                    in_channel = channel < sound.channels ? channel : in_channel;
                    out_channel = channel < m_Have.channels ? channel : out_channel;

                    for (uint32_t i = 0; i < s_chunk; ++i)
                    {
                        combine(work_buffer[i * m_Have.channels + out_channel], chunk[in_channel][i] * it->scale);
                    }

                    ++channel;

                    if (channel >= sound.channels && channel >= m_Have.channels)
                        break;
                }

                if (it->progress == sound.data.size())
                {
                    it = m_ToQueue.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            qlk.unlock();

            for (auto& src : m_Sources)
            {
                src.second(work_buffer.data(), work_buffer.size());
            }

            for (uint32_t i = 0; i < m_buffer.size(); ++i)
            {
                m_buffer[i] = as_output(work_buffer[i]);
            }

            m_ready = true;
        }

        wlk.unlock();
        std::this_thread::yield();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}
