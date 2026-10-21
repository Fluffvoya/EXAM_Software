#pragma once

#include "pages/page.hpp"
#include "object/LED.hpp"

struct PageLED : public Page
{
    PageLED(LED *led);
    void enter() override;
    void exit() override;

private:
    LED *led_;
};
