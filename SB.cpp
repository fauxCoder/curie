#include <Curie/SB.h>

#include <Curie/Quartz.h>

#include <assert.h>

SDL_AudioDeviceID SB::Open(SB* a_SB)
{
    SDL_AudioSpec want;
    memset(&want, 0, sizeof(want));
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
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

    std::unique_lock<std::mutex> lk(sb->m_WriteMutex);

    bool written = false;
    for (auto& c : sb->m_Queue)
    {
        if ( ! c.second.empty())
        {
            if ( ! written)
            {
                memcpy(a_Stream, c.second.front(), a_Length);
                written = true;
            }
            else
            {
                // for (uint32_t i = 0; i < m_SamplesPerFrame; ++i)
                // {
                //     m_Buffer[i*2] += c.second.front()[i*2];
                //     m_Buffer[(i*2)+1] += c.second.front()[(i*2)+1];
                // }
            }

            c.second.pop_front();
        }
    }

    if ( ! written)
    {
        memset(a_Stream, sb->m_Have.silence, a_Length);
    }
}

void SB::Close(SDL_AudioDeviceID a_Device, std::mutex& a_Mutex)
{
    SDL_PauseAudioDevice(a_Device, 0);

    std::unique_lock<std::mutex> lk(a_Mutex);

    SDL_CloseAudioDevice(a_Device);
}

SB::SB(Quartz& a_Q)
: m_Device(Open(this))
{
}

SB::~SB()
{
    Close(m_Device, m_WriteMutex);
}

uint32_t SB::SForF(double a_Frames)
{
    return a_Frames * ((double)m_Have.freq / (double)Quartz::s_FPS);
}

void SB::AddSound(uint32_t a_Key, uint32_t a_CacheSamples, std::function<void(uint32_t, uint32_t, int16_t&, int16_t&)> a_Func)
{
    std::unique_lock<std::mutex> lk(m_WriteMutex);

    uint32_t rounded = ((a_CacheSamples + m_Have.samples) / m_Have.samples) * m_Have.samples;

    std::vector<int16_t> data;
    data.resize(rounded * 2);

    for(uint32_t t = 0; t < rounded; ++t)
    {
        if (t < a_CacheSamples)
        {
            a_Func(t, a_CacheSamples, data[t*2], data[(t*2)+1]);
        }
        else
        {
            data[t*2] = m_Have.silence;
            data[(t*2)+1] = m_Have.silence;
        }
    }

    m_Sounds[a_Key] = data;
}

void SB::PlaySound(uint32_t a_Key, uint32_t a_Channel)
{
    auto s = m_Sounds.find(a_Key);

    if (s != m_Sounds.end())
    {
        std::unique_lock<std::mutex> lk(m_WriteMutex);

        auto c = m_Queue.find(a_Channel);
        if (c == m_Queue.end())
        {
            c = m_Queue.insert({a_Channel, std::deque<int16_t*>()}).first;
        }
        else
        {
            c->second.clear();
        }

        for (int16_t* f = s->second.data(); static_cast<uint32_t>(f - s->second.data()) < s->second.size(); f += (m_Have.samples * m_Have.channels))
        {
            c->second.push_back(f);
        }
    }
}
