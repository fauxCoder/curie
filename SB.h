#pragma once

#include <Curie/Cog.h>
#include <Curie/Sound.h>
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

namespace Curie
{
namespace Snd
{

struct SB
{
    static constexpr double s_rate = 44100.0;

    static working_t as_working(SL::output_t);
    static SL::output_t as_output(working_t);

    static void combine(working_t&, working_t);

    SB(Quartz& a_Q, uint32_t a_channels_out);

    ~SB();

    uint32_t SForF(double);

    std::shared_ptr<Sound> CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&)> a_func);

    std::shared_ptr<Sound> CreateSound(uint32_t a_samples, std::function<void(uint32_t, uint32_t, working_t&, working_t&)> a_func);

    void PlaySound(std::shared_ptr<Sound>&);

    uint32_t AddSource(std::function<void(working_t*, size_t)> a_func);

    void RemoveSource(uint32_t a_Key);

    void Mix();

    Quartz& m_Q;

    uint32_t m_channels_out;

    std::set<std::shared_ptr<Sound>> m_queue;
    std::mutex m_queue_mutex;

    struct Prog
    {
        Prog(std::shared_ptr<Sound>& a_sound, working_t a_scale = 1.0)
        : sound(a_sound)
        , progress(0)
        , scale(a_scale)
        {
        }

        std::shared_ptr<Sound> sound;
        uint32_t progress;
        working_t scale;
    };
    std::list<Prog> m_playing;

    uint32_t m_Start;
    std::map<uint32_t, std::function<void(working_t*, size_t)>> m_Sources;
    std::list<uint32_t> m_Available;

    std::atomic<bool> m_Power;
    std::thread m_Thread;
};

}
}
