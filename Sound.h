#include <array>
#include <vector>

namespace Curie
{
namespace Snd
{

typedef float working_t;

static const size_t s_chunk = 2048;

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

}
}
