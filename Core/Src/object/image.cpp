#include "object/image.hpp"
#include "driver/OLED.hpp"

Image::Image(uint8_t *image, int x, int page,
             int posX, int posPage)
    : image_(image), x_(x), page_(page), pos_x_(posX), pos_page_(posPage) {}

void Image::start()
{
    active = false;
}

void Image::update()
{
    if (!active)
        return;
    OLED::DrawBitmap(image_, x_, page_, pos_x_, pos_page_);
}