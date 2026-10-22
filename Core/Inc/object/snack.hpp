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

enum class SnackState : uint8_t
{
    NO_STATE,
    GAME_OVER,
    GAME_WIN
};

struct Snack : public Object
{
    void start() override;
    void update() override;
    inline uint8_t getSource() { return source_; }
    inline SnackState getState() { return state_; }
    void gameOver();
    void gameWin();
    void genFood();
    void reset();
    inline void turnUp() { dire_ = Direction::UP; }
    inline void turnDown() { dire_ = Direction::DOWN; }
    inline void turnLeft() { dire_ = Direction::LEFT; }
    inline void turnRight() { dire_ = Direction::RIGHT; }

    uint8_t move_after_frames{};

private:
    Food *food_;
    uint8_t source_{};
    std::list<Point> body_{};
    Direction dire_cache_{Direction::UP}; // 缓存上一帧用于判断是否反向
    Direction dire_{Direction::UP};
    uint8_t frame_num_{0};
    SnackState state_{SnackState::NO_STATE};
};