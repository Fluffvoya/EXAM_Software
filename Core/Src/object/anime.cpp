#include "main.h"
#include "object/anime.hpp"
#include "driver/OLED.hpp"

Anime::Anime(uint8_t *frames, uint8_t frame_count, uint8_t fps,
             int x, int page, int pos_x, int pos_page)
    : frames_(frames), frame_count_(frame_count), fps_(fps),
      x_(x), page_(page), pos_x_(pos_x), pos_page_(pos_page) {}

void Anime::start() {}

void Anime::update()
{
    if (hide)
        return;
    auto delta = 1000 / fps_;
    HAL_Delay(delta <= 10 ? delta : delta - 10);
    OLED::DrawBitmap(frames_ + frame_state_ * x_ * page_,
                     x_, page_, pos_x_, pos_page_);
    frame_state_ += 1;
    frame_state_ %= frame_count_;
}