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

    std::unique_lock<std::mutex> lk(sb->m_WriteMutex);

    int32_t to_write = a_Length;

    while (to_write > 0)
    {
        if ( ! sb->m_Queue.empty())
        {
            size_t s = sb->m_Queue.front().size() * sizeof(output_t);
            memcpy(a_Stream, sb->m_Queue.front().data(), s);

            a_Stream += s;
            to_write -= s;

            sb->m_Queue.pop_front();
        }
        else
        {
            memset(a_Stream, sb->m_Have.silence, to_write);
            break;
        }
    }
}

void SB::Close(SDL_AudioDeviceID a_Device, std::mutex& a_Mutex)
{
    SDL_CloseAudioDevice(a_Device);
}

SB::SB(Quartz& a_Q, uint32_t a_Channels)
: m_Q(a_Q)
, m_Device(Open(this, a_Channels))
{
}

SB::~SB()
{
    Close(m_Device, m_WriteMutex);
}

uint32_t SB::SForF(double a_Frames)
{
    return a_Frames * ((double)m_Have.freq / (double)m_Q.m_FPS);
}

uint32_t SB::AddSound(uint32_t a_CacheSamples, std::function<void(uint32_t, uint32_t, working_t&)> a_Func)
{
    std::unique_lock<std::mutex> lk(m_WriteMutex);

    auto& data = m_Sounds.emplace_back();
    data.resize(a_CacheSamples);

    for(uint32_t t = 0; t < a_CacheSamples; ++t)
    {
        a_Func(t, a_CacheSamples, data[t]);
    }

    return m_Sounds.size() - 1;
}

void SB::PlaySound(uint32_t a_Key)
{
    std::unique_lock<std::mutex> lk(m_WriteMutex);

    if (a_Key < m_Sounds.size())
    {

        auto& s = m_Sounds[a_Key];

        auto sample = s.begin();

        auto start = m_Queue.begin();
        uint32_t samples_combined = 0;
        while (start != m_Queue.end() && sample != s.end())
        {
            working_t existing =
                static_cast<working_t>((*start).at(samples_combined))
                / static_cast<working_t>(std::numeric_limits<output_t>::max());

            working_t combined = existing + (*sample) - (existing * (*sample));

            (*start).at(samples_combined) = static_cast<output_t>(combined * static_cast<working_t>(std::numeric_limits<output_t>::max()));

            ++sample;
            ++samples_combined;

            if (samples_combined >= start->size())
            {
                samples_combined = 0;
                ++start;
            }
        }

        std::vector<output_t>* last = nullptr;
        while (sample != s.end())
        {
            if (last == nullptr || last->size() >= m_Have.samples)
            {
                last = &m_Queue.emplace_back();
                last->reserve(m_Have.samples);
            }

            last->emplace_back(static_cast<output_t>((*sample) * static_cast<working_t>(std::numeric_limits<output_t>::max())));
            ++sample;
        }

        while (last && last->size() < m_Have.samples)
        {
            last->emplace_back(m_Have.silence);
        }
    }
}
