#include "main.h"
#include "startup.hpp"
#include "driver/OLED.hpp"
#include "image/kurisu_image.hpp"
#include "image/ui_elements.hpp"

void startup()
{
    OLED::Clear();
    OLED::DrawBitmap(const_cast<uint8_t *>(text_welcome),
                     TEXT_WELCOME_WIDTH, TEXT_PAGES,
                     (OLED::WIDTH - TEXT_WELCOME_WIDTH) / 2,
                     (OLED::PAGES - TEXT_PAGES) / 2);
    OLED::Refresh();
    HAL_Delay(1500);

    OLED::Clear();
    OLED::DrawBitmap(const_cast<uint8_t *>(kurisu_image),
                     KURISU_IMAGE_WIDTH, KURISU_IMAGE_PAGES,
                     (OLED::WIDTH - KURISU_IMAGE_WIDTH) / 2,
                     (OLED::PAGES - KURISU_IMAGE_PAGES) / 2);
    OLED::Refresh();
    HAL_Delay(2000);
}