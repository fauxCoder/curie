#pragma once

#include <cstdint>
#include <memory>

namespace Curie
{
namespace IL
{

enum Input
{
    None = -1,
    Key_1 = 0,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_Return,
    Key_Escape,
    Total,
};

enum Type
{
    KeyDown = 0,
    KeyUp,
};

void prime();

bool poll(Input&, Type&);

}
}
