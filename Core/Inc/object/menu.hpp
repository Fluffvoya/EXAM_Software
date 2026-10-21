#pragma once

#include <stdint.h>
#include "object/object.hpp"
#include "pages/page.hpp"
#include "driver/OLED.hpp"
#include "image/ui_elements.hpp"

template <uint8_t page_num = 1>
class Menu : public Object
{
public:
    Page *pages[page_num];

public:
    Menu() {}

    void start() override {}

    void update() override
    {
        if (is_entered_)
            return;
        showMenu();
    }

    void nextPage()
    {
        curr_page_++;
        curr_page_ %= page_num;
    }
    void prevPage()
    {
        if (curr_page_ == 0)
            curr_page_ = page_num - 1;
        else
            curr_page_--;
        curr_page_ %= page_num;
    }

    void enterPage()
    {
        if (is_entered_)
            return;
        is_entered_ = true;
        pages[curr_page_]->enter();
    }
    void exitPage()
    {
        if (!is_entered_)
            return;
        is_entered_ = false;
        pages[curr_page_]->exit();
    }

private:
    void showMenu()
    {
        OLED::DrawBitmap(pages[curr_page_]->bitmap_, pages[curr_page_]->x_, pages[curr_page_]->page_,
                         (OLED::WIDTH - pages[curr_page_]->x_) / 2,
                         (OLED::PAGES - pages[curr_page_]->page_) / 2 + 1);
        uint8_t *numbers[] = {
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

        OLED::DrawBitmap(numbers[curr_page_], NUMBER_WIDTH, TEXT_PAGES,
                         (OLED::WIDTH - (2 * NUMBER_WIDTH + SYMBOL_SLASH_WIDTH)) / 2,
                         0);
        OLED::DrawBitmap(const_cast<uint8_t *>(symbol_slash), SYMBOL_SLASH_WIDTH, TEXT_PAGES,
                         (OLED::WIDTH - (2 * NUMBER_WIDTH + SYMBOL_SLASH_WIDTH)) / 2 + NUMBER_WIDTH,
                         0);
        OLED::DrawBitmap(numbers[page_num - 1], NUMBER_WIDTH, TEXT_PAGES,
                         (OLED::WIDTH - (2 * NUMBER_WIDTH + SYMBOL_SLASH_WIDTH)) / 2 + NUMBER_WIDTH + SYMBOL_SLASH_WIDTH,
                         0);
    }
    bool is_entered_{false};
    uint8_t curr_page_{0};
};
