#include <Curie/IL.h>

#include <SDL2/SDL.h>

#include <map>
#include <vector>

namespace Curie
{
namespace IL
{

System::System()
{
    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
    {
        printf("SDL_Error: %s\n", SDL_GetError());
    }
}

System::~System()
{
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    SDL_Quit();
}

using SysInput = SDL_Keycode;

static const SysInput nothing = 0;

struct InputPair
{
    Input curie;
    SysInput sys;

    InputPair(Input a_curie, SysInput a_sys)
    : curie(a_curie), sys(a_sys)
    {
    }
};

static const std::vector<InputPair> mapping = {
    { Key_1,        SDLK_1      },
    { Key_2,        SDLK_2      },
    { Key_3,        SDLK_3      },
    { Key_4,        SDLK_4      },
    { Key_5,        SDLK_5      },
    { Key_Return,   SDLK_RETURN },
    { Key_Escape,   SDLK_ESCAPE },
};

SysInput map(Input input)
{
    if (input >= 0 && input < Total && mapping[input].curie == input)
    {
        return mapping[input].sys;
    }

    return nothing;
}

Input reverse(SysInput sys_input)
{
    static std::vector<InputPair> reverse_mapping;
    static uint32_t offset = 0;

    if (reverse_mapping.empty())
    {
        std::map<SysInput, Input> progress;

        for (uint32_t e = 0; e < Total; ++e)
        {
            progress[map(static_cast<Input>(e))] = static_cast<Input>(e);
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

    uint32_t index = sys_input - offset;
    if (index < reverse_mapping.size() && reverse_mapping[index].sys == sys_input)
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

bool poll(Input& o_input, Type& o_type)
{
    SDL_Event e;
    bool ret = SDL_PollEvent(&e) != 0;

    if (ret)
    {
        o_input = reverse(e.key.keysym.sym);

        if (e.type == SDL_KEYDOWN)
            o_type = KeyDown;
        else if (e.type == SDL_KEYUP)
            o_type = KeyUp;
    }

    return ret;
}

}
}
