#include <Curie/SL.h>

#include <portaudio.h>

#include <cassert>

namespace Curie
{
namespace SL
{

struct Stream::impl
{
    PaStreamParameters output;
    PaStream* stream = nullptr;
};

Stream::Stream(double a_rate, uint32_t a_channels, std::size_t a_chunk)
: pimpl(std::make_unique<impl>())
{
    auto error = Pa_Initialize();
    assert(error == paNoError);

    pimpl->output.device = Pa_GetDefaultOutputDevice();
    pimpl->output.channelCount = a_channels;
    pimpl->output.sampleFormat = paInt16;
    pimpl->output.suggestedLatency = Pa_GetDeviceInfo(pimpl->output.device)->defaultLowOutputLatency;
    pimpl->output.hostApiSpecificStreamInfo = nullptr;
    error = Pa_OpenStream(&pimpl->stream, nullptr, &pimpl->output, a_rate, a_chunk, paNoFlag, nullptr, nullptr);
    assert(error == paNoError);

    error = Pa_StartStream(pimpl->stream);
    assert(error == paNoError);
}

Stream::~Stream()
{
    auto error = Pa_StopStream(pimpl->stream);
    assert(error == paNoError);

    error = Pa_CloseStream(pimpl->stream);
    assert(error == paNoError);

    error = Pa_Terminate();
    assert(error == paNoError);
}

void Stream::write(output_t* a_data, std::size_t a_size)
{
    auto error = Pa_WriteStream(pimpl->stream, a_data, a_size);
    assert(error == paNoError);
}

uint32_t Stream::channels_out()
{
    return pimpl->output.channelCount;
}

}
}
