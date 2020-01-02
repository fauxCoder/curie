#pragma once

#include <Curie/Cog.h>

#include <SDL2/SDL.h>

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
    typedef int16_t output_t;

    static SDL_AudioDeviceID Open(SB*, uint32_t a_channels);

    static void Write(void* a_SB, uint8_t* a_Stream, int32_t a_Length);

    static void Close(SDL_AudioDeviceID);

    static working_t as_working(output_t);
    static output_t as_output(working_t);

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

    SDL_AudioDeviceID m_Device;
    SDL_AudioSpec m_Have;

    struct Sound
    {
        Sound(uint32_t a_channels)
        : channels(a_channels)
        {
        }

        std::vector<std::vector<working_t>>& extend(size_t chunk)
        {
            auto& ret = data.emplace_back(channels);

            for (auto& c : ret)
            {
                c.resize(chunk);
                std::fill(c.begin(), c.end(), 0.0);
            }

            return ret;
        }

        uint32_t channels;
        std::vector<std::vector<std::vector<working_t>>> data;
    };
    std::vector<Sound> m_Sounds;

    std::mutex m_QueueMutex;
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
    std::list<Prog> m_ToQueue;

    uint8_t m_Start;
    std::map<uint8_t, std::function<void(working_t*, size_t)>> m_Sources;
    std::list<uint8_t> m_Available;

    std::mutex m_write_mutex;
    std::vector<output_t> m_buffer;
    bool m_ready;

    std::atomic<bool> m_Power;
    std::thread m_Thread;
};
