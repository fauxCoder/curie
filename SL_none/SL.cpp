#include <Curie/SL.h>

#include <portaudio.h>

#include <cassert>
#include <unistd.h>

namespace Curie
{
namespace SL
{

struct Stream::impl
{
};

Stream::Stream(double a_rate, uint32_t a_channels, std::size_t a_chunk)
: pimpl(std::make_unique<impl>())
{
}

Stream::~Stream()
{
}

void Stream::write(output_t* a_data, std::size_t a_size)
{
}

uint32_t Stream::channels_out()
{
    return 0;
}

}
}
