#pragma once

#include <Curie/Cog.h>
#include <Curie/SL.h>

#include <atomic>
#include <deque>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <vector>

struct Quartz;

struct SB
{
    typedef float working_t;

    static constexpr double s_rate = 44100.0;
    static const size_t s_chunk = 2048;

    static working_t as_working(SL::output_t);
    static SL::output_t as_output(working_t);

    static void combine(working_t&, working_t);

    SB(Quartz& a_Q, uint32_t a_channels_out);

    ~SB();

    uint32_t SForF(double);

    uint32_t CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&)> a_func);

    uint32_t CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&, working_t&)> a_func);

    void PlaySound(uint32_t a_Key);

    uint8_t AddSource(std::function<void(working_t*, size_t)> a_func);

    void RemoveSource(uint8_t a_Key);

    void Mix();

    Quartz& m_Q;

    uint32_t m_channels_out;

    struct Sound
    {
        Sound(uint32_t a_channels)
        : channels(a_channels)
        {
        }

        std::vector<std::array<working_t, s_chunk>>& extend(size_t chunk)
        {
            auto& ret = data.emplace_back(channels);

            for (auto& c : ret)
            {
                std::fill(c.begin(), c.end(), 0.0);
            }

            return ret;
        }

        uint32_t channels;
        std::vector<std::vector<std::array<working_t, s_chunk>>> data;
    };
    std::vector<Sound> m_Sounds;

    std::set<uint32_t> m_queue;
    std::mutex m_queue_mutex;

    struct Prog
    {
        Prog(uint32_t a_key, working_t a_scale = 1.0)
        : key(a_key)
        , progress(0)
        , scale(a_scale)
        {
        }

        uint32_t key;
        uint32_t progress;
        working_t scale;
    };
    std::list<Prog> m_playing;

    uint8_t m_Start;
    std::map<uint8_t, std::function<void(working_t*, size_t)>> m_Sources;
    std::list<uint8_t> m_Available;

    std::atomic<bool> m_Power;
    std::thread m_Thread;
};
