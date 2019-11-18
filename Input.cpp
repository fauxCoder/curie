#include <Curie/Input.h>

void Input::open(std::function<void(void)> a_DefaultResponse)
{
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_KEYUP);
    SDL_FlushEvent(SDL_KEYDOWN);

    bool done = false;
    while ( ! done)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_KEYUP)
            {
                for (auto& kdr : m_KeyUpResponses)
                {
                    for (auto k : kdr.first.codes)
                    {
                        if (k == e.key.keysym.sym)
                        {
                            done = kdr.second(k);
                            break;
                        }
                    }
                }
            }
            else if (e.type == SDL_KEYDOWN)
            {
                for (auto& kdr : m_KeyDownResponses)
                {
                    for (auto k : kdr.first.codes)
                    {
                        if (k == e.key.keysym.sym)
                        {
                            done = kdr.second(k);
                            break;
                        }
                    }
                }
            }
        }

        a_DefaultResponse();
    }
}
