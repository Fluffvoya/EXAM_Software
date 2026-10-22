#pragma once

#include <stdint.h>
#include "object.hpp"

struct Image : public Object
{
    Image(uint8_t *image, int x, int page,
          int posX, int posPage);
    void start() override;
    void update() override;

private:
    uint8_t *image_;
    int x_, page_,
        pos_x_, pos_page_;
};
