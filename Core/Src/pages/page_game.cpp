#include "pages/page_game.hpp"
#include "image/ui_elements.hpp"
#include "driver/OLED.hpp"
#include "image/ui_elements.hpp"
#include "object_list.hpp"
#include <stdint.h>

void PageGame::start()
{
    active = false;
    snack_ = &ObjectList::snack;
    food_ = &ObjectList::food;
    bitmap_ = const_cast<uint8_t *>(text_game);
    x_ = TEXT_GAME_WIDTH;
    page_ = TEXT_PAGES;
}
void PageGame::update()
{
    if (!active)
        return;
    OLED::DrawRectangle(74, 0, 2, 64, 1, 1);
    OLED::DrawBitmap(const_cast<uint8_t *>(text_source),
                     TEXT_SOURCE_WIDTH, TEXT_PAGES,
                     (OLED::WIDTH - 76 - TEXT_SOURCE_WIDTH) / 2 + 76,
                     0);
    uint8_t *numbers[] = {
        const_cast<uint8_t *>(number_0),
        const_cast<uint8_t *>(number_1),
        const_cast<uint8_t *>(number_2),
        const_cast<uint8_t *>(number_3),
        const_cast<uint8_t *>(number_4),
        const_cast<uint8_t *>(number_5),
        const_cast<uint8_t *>(number_6),
        const_cast<uint8_t *>(number_7),
        const_cast<uint8_t *>(number_8),
        const_cast<uint8_t *>(number_9),
    };
    uint8_t unit = snack_->getSource() % 10;
    uint8_t ten = (snack_->getSource() / 10) % 10;
    OLED::DrawBitmap(numbers[unit], NUMBER_WIDTH, TEXT_PAGES,
                     (OLED::WIDTH - 76 - NUMBER_WIDTH * 2) / 2 + 76,
                     3);
    OLED::DrawBitmap(numbers[ten], NUMBER_WIDTH, TEXT_PAGES,
                     (OLED::WIDTH - 76 - NUMBER_WIDTH * 2) / 2 + 76 + NUMBER_WIDTH,
                     3);
}

void PageGame::enter()
{
    active = true;
    snack_->active = true;
    food_->active = true;
}

void PageGame::exit()
{
    active = false;
    snack_->active = false;
    food_->active = false;
}