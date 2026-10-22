#pragma once

#include "pages/page.hpp"
#include "object/LED.hpp"

struct PageLED : public Page, public Object
{
    void start() override;
    void update() override;
    void enter() override;
    void exit() override;

private:
    LED *led_{};
};
