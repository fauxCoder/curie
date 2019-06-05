#include <Curie/Input.h>
#include <Curie/Quartz.h>

void Input::Enter(std::function<void(void)> a_DefaultResponse)
{
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_KEYDOWN);

    bool done = false;
    while ( ! done)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_KEYDOWN)
            {
                bool found = false;
                for (auto& kdr : m_KeyDownResponses)
                {
                    for (auto k : kdr.first)
                    {
                        if (k == e.key.keysym.sym)
                        {
                            done = kdr.second(k);
                            found = true;
                            break;
                        }
                    }

                    if (found)
                    {
                        break;
                    }
                }
            }
        }

        a_DefaultResponse();

        m_Q.Tooth();
    }
}
