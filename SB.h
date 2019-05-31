#pragma once

#include <Curie/Cog.h>

#include <SDL2/SDL.h>

#include <deque>
#include <functional>
#include <map>
#include <vector>

struct Quartz;

struct SB
{
    typedef double working_t;
    typedef int16_t output_t;

    static SDL_AudioDeviceID Open(SB* a_SB, uint32_t a_Channels);

    static void Write(void* a_SB, uint8_t* a_Stream, int32_t a_Length);

    static void Close(SDL_AudioDeviceID, std::mutex& a_Mutex);

    SB(Quartz& a_Q, uint32_t a_Channels);

    ~SB();

    uint32_t SForF(double a_Frames);

    void AddSound(uint32_t a_Key, uint32_t a_CacheSamples, std::function<void(uint32_t, uint32_t, working_t&)> a_Func);

    void PlaySound(uint32_t a_Key);

    Quartz& m_Q;

    SDL_AudioDeviceID m_Device;

    SDL_AudioSpec m_Have;

    std::mutex m_WriteMutex;
    std::map<uint32_t, std::vector<working_t>> m_Sounds;
    std::deque<std::vector<output_t>> m_Queue;
};
