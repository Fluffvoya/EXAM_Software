#include "pages/page_game.hpp"
#include "image/ui_elements.hpp"
#include <stdint.h>

PageGame::PageGame()
{
    bitmap_ = const_cast<uint8_t *>(text_game);
    x_ = TEXT_GAME_WIDTH;
    page_ = TEXT_PAGES;
}

void PageGame::enter() {}

void PageGame::exit()
{
}