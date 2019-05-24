#include <Curie/SB.h>

#include <Curie/Quartz.h>

#include <cassert>

SDL_AudioDeviceID SB::Open(SB* a_SB, uint32_t a_Channels)
{
    SDL_AudioSpec want;
    memset(&want, 0, sizeof(want));
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = a_Channels;
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

    if (sb->m_Queue.size() > 0)
    {
        memcpy(a_Stream, sb->m_Queue.front().data(), a_Length);
        sb->m_Queue.pop_front();
    }
    else
    {
        memset(a_Stream, sb->m_Have.silence, a_Length);
    }
}

void SB::Close(SDL_AudioDeviceID a_Device, std::mutex& a_Mutex)
{
    SDL_CloseAudioDevice(a_Device);
}

SB::SB(Quartz& a_Q, uint32_t a_Channels)
// : m_Device(Open(this, a_Channels))
{
}

SB::~SB()
{
    // Close(m_Device, m_WriteMutex);
}

uint32_t SB::SForF(double a_Frames)
{
    return a_Frames * ((double)m_Have.freq / (double)Quartz::s_FPS);
}

void SB::AddSound(uint32_t a_Key, uint32_t a_CacheSamples, std::function<void(uint32_t, uint32_t, working_t&)> a_Func)
{
    std::unique_lock<std::mutex> lk(m_WriteMutex);

    std::vector<working_t> data;
    data.resize(a_CacheSamples);

    for(uint32_t t = 0; t < a_CacheSamples; ++t)
    {
        a_Func(t, a_CacheSamples, data[t]);
    }

    m_Sounds[a_Key] = data;
}

void SB::PlaySound(uint32_t a_Key)
{
    auto s = m_Sounds.find(a_Key);

    if (s != m_Sounds.end())
    {
        std::unique_lock<std::mutex> lk(m_WriteMutex);

        auto last = &m_Queue.emplace_back();
        last->reserve(m_Have.samples);
        for (auto& sample : s->second)
        {
            if (last->size() >= m_Have.samples)
            {
                last = &m_Queue.emplace_back();
            }

            last->emplace_back(static_cast<output_t>(sample * static_cast<working_t>(std::numeric_limits<output_t>::max())));
        }

        while (last->size() < m_Have.samples)
        {
            last->push_back(m_Have.silence);
        }
    }
}
