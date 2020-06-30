#include <Curie/Cam.h>

namespace Curie
{

void Cam::open(std::function<void(void)> a_DefaultResponse)
{
    IL::prime();

    bool done = false;
    while ( ! done)
    {
        IL::Input e = IL::None;
        IL::Type type = IL::KeyDown;
        while (IL::poll(e, type))
        {
            if (type == IL::KeyUp)
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
            else if (type == IL::KeyDown)
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
