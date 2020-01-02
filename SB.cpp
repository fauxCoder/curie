#include <Curie/SB.h>

#include <Curie/Quartz.h>

#include <algorithm>
#include <cassert>
#include <chrono>

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
, m_channels_out(a_channels_out)
, m_stream(nullptr)
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
    return a_Frames * (44100.0 / (double)m_Q.m_FPS);
}

uint32_t SB::CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&)> a_func)
{
    auto& sound = m_Sounds.emplace_back(1);

    uint32_t so_far = 0;
    while (so_far < a_samples)
    {
        auto& data = sound.extend(s_chunk);

        for (uint32_t t = 0; t < s_chunk; ++t)
        {
            if (so_far < a_samples)
            {
                a_func(so_far, a_samples, data[0][t]);
                ++so_far;
            }
        }
    }

    return m_Sounds.size() - 1;
}

uint32_t SB::CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&, working_t&)> a_func)
{
    auto& sound = m_Sounds.emplace_back(2);

    uint32_t so_far = 0;
    while (so_far < a_samples)
    {
        auto& data = sound.extend(s_chunk);

        for (uint32_t t = 0; t < s_chunk; ++t)
        {
            if (so_far < a_samples)
            {
                a_func(so_far, a_samples, data[0][t], data[1][t]);
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
    auto error = Pa_Initialize();
    assert(error == paNoError);

    PaStreamParameters output;
    output.device = Pa_GetDefaultOutputDevice();
    output.channelCount = m_channels_out;
    output.sampleFormat = paInt16;
    output.suggestedLatency = Pa_GetDeviceInfo(output.device)->defaultLowOutputLatency;
    error = Pa_OpenStream(&m_stream, nullptr, &output, 44100.0, s_chunk, paNoFlag, nullptr, nullptr);
    assert(error == paNoError);

    error = Pa_StartStream(m_stream);
    assert(error == paNoError);

    std::vector<working_t> work_buffer;
    work_buffer.resize(s_chunk * output.channelCount);

    std::vector<output_t> out_buffer;
    out_buffer.resize(s_chunk * output.channelCount);

    while (m_Power.load())
    {
        std::fill(work_buffer.begin(), work_buffer.end(), 0.0);

        std::unique_lock<std::mutex> qlk(m_QueueMutex);

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
                out_channel = channel < output.channelCount ? channel : out_channel;

                for (uint32_t i = 0; i < s_chunk; ++i)
                {
                    combine(work_buffer[i * output.channelCount + out_channel], chunk[in_channel][i] * it->scale);
                }

                ++channel;

                if (channel >= sound.channels && channel >= output.channelCount)
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

        for (uint32_t i = 0; i < out_buffer.size(); ++i)
        {
            out_buffer[i] = as_output(work_buffer[i]);
        }

        error = Pa_WriteStream(m_stream, out_buffer.data(), s_chunk);
    }

    error = Pa_StopStream(m_stream);
    assert(error == paNoError);

    error = Pa_CloseStream(m_stream);
    assert(error == paNoError);

    error = Pa_Terminate();
    assert(error == paNoError);
}
