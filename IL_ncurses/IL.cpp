#include <Curie/IL.h>

#include <curses.h>

#include <map>
#include <vector>

namespace Curie
{
namespace IL
{

System::System()
{
    initscr();
    cbreak();
    noecho();
}

System::~System()
{
    endwin();
}

using SysInput = int;

static const std::map<SysInput, Input> mapping = {
    { '1',          Key_1       },
    { '2',          Key_2       },
    { '3',          Key_3       },
    { '4',          Key_4       },
    { '5',          Key_5       },
    { KEY_ENTER,    Key_Return  },
    { 'q',          Key_Escape  },
};

void prime()
{
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
}

bool poll(Input& o_input, Type& o_type)
{
    auto ch = getch();

    if (ch != ERR)
    {
        auto found = mapping.find(ch);
        if (found != mapping.end())
        {
            o_input = found->second;
            o_type = KeyDown;
            flushinp();
            return true;
        }
    }

    flushinp();
    return false;
}

}
}
