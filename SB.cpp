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

    int32_t togo = a_Length;

    while (togo > 0)
    {
        if ( ! sb->m_ToWrite.empty())
        {
            size_t s = sb->m_ToWrite.front().size() * sizeof(output_t);
            memcpy(a_Stream, sb->m_ToWrite.front().data(), s);

            a_Stream += s;
            togo -= s;

            sb->m_ToWrite.pop_front();
        }
        else
        {
            memset(a_Stream, sb->m_Have.silence, togo);
            break;
        }
    }
}

void SB::Close(SDL_AudioDeviceID a_Device, std::mutex& a_Mutex)
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

SB::SB(Quartz& a_Q, uint32_t a_Channels)
: m_Q(a_Q)
, m_Device(Open(this, a_Channels))
, m_Power(true)
, m_Thread(&SB::QueueSound, this)
{
}

SB::~SB()
{
    Close(m_Device, m_WriteMutex);

    m_Power.store(false);

    m_Thread.join();
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
    std::unique_lock<std::mutex> lk(m_QueueMutex);

    m_ToQueue.push_back(a_Key);
}

void SB::QueueSound()
{
    while (m_Power.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));

        std::unique_lock<std::mutex> qlk(m_QueueMutex);

        while ( ! m_ToQueue.empty())
        {
            auto key = m_ToQueue.back();
            m_ToQueue.pop_back();

            qlk.unlock();

            if (key < m_Sounds.size())
            {
                auto& sound = m_Sounds[key];

                auto sample = sound.begin();

                std::unique_lock<std::mutex> wlk(m_WriteMutex);

                auto start = m_ToWrite.begin();
                uint32_t samples_combined = 0;
                while (start != m_ToWrite.end() && sample != sound.end())
                {
                    working_t combined = combine(as_working((*start).at(samples_combined)), (*sample));

                    start->at(samples_combined) = as_output(combined);

                    ++sample;
                    ++samples_combined;

                    if (samples_combined >= start->size())
                    {
                        samples_combined = 0;
                        ++start;
                    }
                }

                std::vector<output_t>* last = nullptr;
                while (sample != sound.end())
                {
                    if (last == nullptr || last->size() >= m_Have.samples)
                    {
                        last = &m_ToWrite.emplace_back();
                        last->reserve(m_Have.samples);
                    }

                    last->emplace_back(as_output(*sample));
                    ++sample;
                }

                while (last && last->size() < m_Have.samples)
                {
                    last->emplace_back(m_Have.silence);
                }
            }

            qlk.lock();
        }
    }
}
