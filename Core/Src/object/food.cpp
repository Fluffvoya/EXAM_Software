#include "object/food.hpp"
#include "driver/OLED.hpp"

void Food::start()
{
    active = false;
    is_win_or_lose_ = false;
}

void Food::update()
{
    if (!active)
        return;
    if (is_win_or_lose_)
        return;
    OLED::DrawCircle(pos_.x * 8 + 4, pos_.y * 8 + 4, 3, 1, 1);
}