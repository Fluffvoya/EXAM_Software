#include "pages/page_led.hpp"
#include "driver/OLED.hpp"
#include "image/ui_elements.hpp"

PageLED::PageLED(LED *led)
    : led_(led)
{
    bitmap_ = const_cast<uint8_t *>(text_led);
    x_ = TEXT_LED_WIDTH;
}
void PageLED::enter()
{
    led_->light = true;
}

void PageLED::exit()
{
    led_->light = false;
}