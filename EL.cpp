#include <Curie/EL.h>

#include <SDL2/SDL.h>

#include <map>
#include <vector>

namespace Curie
{
namespace EL
{

using SysEvent = SDL_Keycode;

static const SysEvent nothing = 0;

struct EventPair
{
    Event curie;
    SysEvent sys;

    EventPair(Event a_curie, SysEvent a_sys)
    : curie(a_curie), sys(a_sys)
    {
    }
};

static const std::vector<EventPair> mapping = {
    { Key_1,        SDLK_1      },
    { Key_2,        SDLK_2      },
    { Key_3,        SDLK_3      },
    { Key_4,        SDLK_4      },
    { Key_5,        SDLK_5      },
    { Key_Return,   SDLK_RETURN },
    { Key_Escape,   SDLK_ESCAPE },
};

SysEvent map(Event event)
{
    if (event >= 0 && event < Total && mapping[event].curie == event)
    {
        return mapping[event].sys;
    }

    return nothing;
}

Event reverse(SysEvent sys_event)
{
    static std::vector<EventPair> reverse_mapping;
    static uint32_t offset = 0;

    if (reverse_mapping.empty())
    {
        std::map<SysEvent, Event> progress;

        for (uint32_t e = 0; e < Total; ++e)
        {
            progress[map(static_cast<Event>(e))] = static_cast<Event>(e);
        }

        offset = progress.begin()->first;

        uint32_t largest = progress.rbegin()->first;

        for (uint32_t i = offset; i <= largest; ++i)
        {
            auto it = progress.find(i);

            if (it != progress.end())
            {
                reverse_mapping.emplace_back(it->second, i);
            }
            else
            {
                reverse_mapping.emplace_back(None, i);
            }
        }
    }

    uint32_t index = sys_event - offset;
    if (index < reverse_mapping.size() && reverse_mapping[index].sys == sys_event)
    {
        return reverse_mapping[index].curie;
    }

    return None;
}

void prime()
{
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_KEYUP);
    SDL_FlushEvent(SDL_KEYDOWN);
}

bool poll(Event& o_event, Type& o_type)
{
    SDL_Event e;
    bool ret = SDL_PollEvent(&e) != 0;

    if (ret)
    {
        o_event = reverse(e.key.keysym.sym);

        if (e.type == SDL_KEYDOWN)
            o_type = KeyDown;
        else if (e.type == SDL_KEYUP)
            o_type = KeyUp;
    }

    return ret;
}

}
}
