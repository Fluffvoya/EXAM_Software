#pragma once

#include "object/object.hpp"
#include "object/food.hpp"
#include <list>
#include <stdint.h>

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
    inline uint8_t getSource() { return source_; }
    void gameOver();
    void gameWin();
    void genFood();

    uint8_t move_after_frames{};

private:
    Food *food_;
    uint8_t source_{};
    std::list<Point> body_{};
    Direction dire_cache_{Direction::UP}; // 缓存上一帧用于判断是否反向
    Direction dire_{Direction::UP};
    uint8_t frame_num_{0};
};