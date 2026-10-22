#pragma once

#include "object/button.hpp"
#include "object/LED.hpp"
#include "object/menu.hpp"
#include "object/image.hpp"
#include "object/anime.hpp"

class ObjectList
{
public:
    static Button button_back;
    static Button button_prev;
    static Button button_next;
    static Button button_ok;

    static Button button_left;
    static Button button_up;
    static Button button_down;
    static Button button_right;

    static LED led;
    static Image image;
    static Anime anime;

    static Menu<4> menu;
};