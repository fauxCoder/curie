#include <Curie/Input.h>

namespace Curie
{

void Input::open(std::function<void(void)> a_DefaultResponse)
{
    EL::prime();

    bool done = false;
    while ( ! done)
    {
        EL::Event e = EL::None;
        EL::Type type = EL::KeyDown;
        while (EL::poll(e, type))
        {
            if (type == EL::KeyUp)
            {
                for (auto& kdr : m_KeyUpResponses)
                {
                    for (auto k : kdr.first.codes)
                    {
                        if (k == e)
                        {
                            done = kdr.second(k);
                            break;
                        }
                    }
                }
            }
            else if (type == EL::KeyDown)
            {
                for (auto& kdr : m_KeyDownResponses)
                {
                    for (auto k : kdr.first.codes)
                    {
                        if (k == e)
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

}
