#include "object/snack.hpp"
#include "driver/OLED.hpp"
#include "object_list.hpp"
#include <stdlib.h>

void Snack::start()
{
    active = false;
    food_ = &ObjectList::food;
    move_after_frames = 20;
    body_.push_back({3, 6});
    body_.push_back({3, 7});
    srand(*(volatile uint16_t *)0x000008);
    genFood();
}

void Snack::update()
{
    if (!active)
        return;

    if (frame_num_ < move_after_frames)
    {
        frame_num_++;
    }
    else
    {
        frame_num_ = 0;
        // 往后方转，撤销本次操作
        if (static_cast<int8_t>(dire_cache_) == static_cast<int8_t>(dire_))
            dire_ = dire_cache_;
        else
            dire_cache_ = dire_;

        auto last = --body_.end();
        auto first = body_.begin();
        uint8_t x{first->x}, y{first->y};
        switch (dire_)
        {
        case Direction::UP:
            y--;
            break;
        case Direction::DOWN:
            y++;
            break;
        case Direction::LEFT:
            x--;
            break;
        case Direction::RIGHT:
            x++;
            break;
        default:
            break;
        }
        *last = {x, y};
        // 移动
        body_.splice(first, body_, last);
    }

    // 绘制
    for (auto i : body_)
    {
        OLED::DrawRectangle(i.x * 8, i.y * 8, 8, 8, 1, 1);
    }
}

void Snack::genFood()
{
    uint8_t total{};

    auto is_on_body =
        [this](uint8_t i, uint8_t j)
    {
        for (auto item : body_)
        {
            if (item.x == i && item.y == j)
                return true;
        }
        return false;
    };

    for (uint8_t i = 0; i < 9; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            if (!is_on_body(i, j))
                total++;
        }
    }

    uint8_t food_pos = rand() % total;
    for (uint8_t i = 0; i < 9; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            if (!is_on_body(i, j))
            {
                if (food_pos == 0)
                {
                    food_->pos_ = {i, j};
                    return;
                }
                food_pos--;
            }
        }
    }
}