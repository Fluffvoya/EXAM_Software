#pragma once

#include "object/object.hpp"
#include <stdint.h>

struct Point
{
    int8_t x{}, y{};
};

struct Food : public Object
{
    friend struct Snack;
    void start() override;
    void update() override;

private:
    // managed by Snack
    Point pos_{};
    bool is_win_or_lose_{};
};
