#pragma once

#include "object.hpp"
#include <stdint.h>

struct Anime : public Object
{
    Anime(uint8_t *frames, uint8_t frame_count, uint8_t fps,
          int x, int page, int pos_x, int pos_page);

    void start() override;
    void update() override;

private:
    uint8_t *frames_;
    uint8_t frame_count_, frame_state_{0}, fps_;
    int x_, page_, pos_x_, pos_page_;
};
