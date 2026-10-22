#include "pages/page_led.hpp"
#include "driver/OLED.hpp"
#include "image/ui_elements.hpp"
#include "object_list.hpp"

void PageLED::start()
{
    bitmap_ = const_cast<uint8_t *>(text_led);
    x_ = TEXT_LED_WIDTH;
    page_ = TEXT_PAGES;
    active = false;
    led_ = &ObjectList::led;
}

void PageLED::update()
{
    if (!active)
        return;
    OLED::DisClear();
}

void PageLED::enter()
{
    led_->active = true;
    active = true;
}

void PageLED::exit()
{
    led_->active = false;
    active = false;
}