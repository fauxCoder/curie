#pragma once

#include <portaudio.h>

#include <cstdint>

namespace SL
{

typedef int16_t output_t;

struct Stream
{
    Stream(double a_rate, uint32_t a_channels, std::size_t a_chunk);
    ~Stream();

    void write(output_t*, std::size_t);

    uint32_t channels_out();

    PaStreamParameters _output;
    PaStream* _stream;
};

}
