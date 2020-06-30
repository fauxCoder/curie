#pragma once

#include <string>
#include <memory>

namespace Curie
{
namespace VL
{

struct System
{
    System();
    ~System();
};

struct Image;

struct Rect
{
    union
    {
        int rect[4];
        struct
        {
            int x, y, w, h;
        };
    };
};

struct Window
{
    Window();
    ~Window();

    void init();

    void clear();

    void copy(Image&, Rect&);

    void present();

    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct Image
{
    Image(std::string);

    ~Image();

    void init(Window&);

    int32_t get_width();

    int32_t get_height();

    struct impl;
    std::unique_ptr<impl> pimpl;
};

}
}
