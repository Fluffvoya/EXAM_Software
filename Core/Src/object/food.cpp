#include "object/food.hpp"
#include "driver/OLED.hpp"

void Food::start()
{
    active = false;
}

void Food::update()
{
    if (!active)
        return;
    OLED::DrawCircle(pos_.x * 8 + 4, pos_.y * 8 + 4, 3, 1, 1);
}