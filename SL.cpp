#include <Curie/SL.h>

#include <cassert>

namespace SL
{

Stream::Stream(double a_rate, uint32_t a_channels, std::size_t a_chunk)
: _stream(nullptr)
{
    auto error = Pa_Initialize();
    assert(error == paNoError);

    _output.device = Pa_GetDefaultOutputDevice();
    _output.channelCount = a_channels;
    _output.sampleFormat = paInt16;
    _output.suggestedLatency = Pa_GetDeviceInfo(_output.device)->defaultLowOutputLatency;
    _output.hostApiSpecificStreamInfo = nullptr;
    error = Pa_OpenStream(&_stream, nullptr, &_output, a_rate, a_chunk, paNoFlag, nullptr, nullptr);
    assert(error == paNoError);

    error = Pa_StartStream(_stream);
    assert(error == paNoError);
}

Stream::~Stream()
{
    auto error = Pa_StopStream(_stream);
    assert(error == paNoError);

    error = Pa_CloseStream(_stream);
    assert(error == paNoError);

    error = Pa_Terminate();
    assert(error == paNoError);
}

void Stream::write(output_t* a_data, std::size_t a_size)
{
    auto error = Pa_WriteStream(_stream, a_data, a_size);
    assert(error == paNoError);
}

uint32_t Stream::channels_out()
{
    return _output.channelCount;
}

}
