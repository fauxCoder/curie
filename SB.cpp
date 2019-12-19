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

SB::working_t SB::combine(working_t a_A, working_t a_B)
{
    working_t ret = a_A + a_B;

    working_t correction = abs(a_A) * abs(a_B);
    if (a_A > 0.0 && a_B > 0.0)
    {
        ret -= correction;
    }
    else if (a_A < 0.0 && a_B < 0.0)
    {
        ret += correction;
    }

    return ret;
}

SB::SB(Quartz& a_Q, uint32_t a_channels_in, uint32_t a_channels_out)
: m_Q(a_Q)
, m_Device(Open(this, a_channels_out))
, m_Start(1)
, m_buffer(m_Have.samples * m_Have.channels)
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

uint32_t SB::CreateSound(uint32_t a_CacheSamples, std::function<void(uint32_t, uint32_t, working_t*)> a_Func)
{
    auto& sound = m_Sounds.emplace_back();

    uint32_t so_far = 0;
    while (so_far < a_CacheSamples)
    {
        auto& data = sound.emplace_back(m_Have.samples * m_Have.channels);

        for(uint32_t t = 0; t < m_Have.samples; ++t)
        {
            if (so_far < a_CacheSamples)
            {
                a_Func(so_far, a_CacheSamples, &data[t * m_Have.channels]);
                ++so_far;
            }
        }
    }

    return m_Sounds.size() - 1;
}

void SB::PlaySound(uint32_t a_Key)
{
    std::unique_lock<std::mutex> lk(m_QueueMutex);

    m_ToQueue.emplace_back(a_Key);
}

uint8_t SB::AddSource(std::function<void(working_t*)> a_func)
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
    while (m_Power.load())
    {
        std::unique_lock<std::mutex> wlk(m_write_mutex);

        if (!m_ready)
        {
            std::list<std::vector<working_t>*> all_sounds;

            std::unique_lock<std::mutex> qlk(m_QueueMutex);
            auto it = m_ToQueue.begin();
            while (it != m_ToQueue.end())
            {
                auto& sound = m_Sounds[it->key];
                all_sounds.push_back(&sound[it->progress++]);

                if (it->progress == sound.size())
                {
                    it = m_ToQueue.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            qlk.unlock();

            for (uint32_t i = 0; i < m_buffer.size(); i += 2)
            {
                uint32_t lefti = i;
                uint32_t righti = i + 1;

                working_t left = 0;
                working_t right = 0;

                for (auto s : all_sounds)
                {
                    left = combine(left, (*s)[lefti]);
                    right = combine(right, (*s)[righti]);
                }

                for (auto& src : m_Sources)
                {
                    working_t out[2];
                    src.second(out);
                    left = combine(left, out[0]);
                    right = combine(right, out[1]);
                }

                m_buffer[lefti] = as_output(left);
                m_buffer[righti] = as_output(right);
            }

            m_ready = true;
        }

        wlk.unlock();
        std::this_thread::yield();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}
