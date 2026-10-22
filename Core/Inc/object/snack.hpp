#pragma once

#include "object/object.hpp"
#include <list>
#include <stdint.h>

struct Point
{
    uint8_t x{}, y{};
};

enum class Direction : int8_t
{
    UP = -1,
    DOWN = 1,
    LEFT = -2,
    RIGHT = 2,
};

struct Snack : public Object
{
    void start() override;
    void update() override;

    uint8_t move_after_frames{};

private:
    std::list<Point> body_{};
    Direction dire_cache_{Direction::UP}; // 缓存上一帧用于判断是否反向
    Direction dire_{Direction::UP};
    uint8_t frame_num_{0};
};