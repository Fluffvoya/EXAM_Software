#include "pages/page_game.hpp"
#include "image/ui_elements.hpp"
#include <stdint.h>

void PageGame::start()
{
    bitmap_ = const_cast<uint8_t *>(text_game);
    x_ = TEXT_GAME_WIDTH;
    page_ = TEXT_PAGES;
    active = false;
}
void PageGame::update() {}

void PageGame::enter()
{
    active = true;
}

void PageGame::exit()
{
    active = true;
}