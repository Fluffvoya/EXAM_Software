#pragma once

#include "pages/page.hpp"
#include "object/snack.hpp"
#include "object/LED.hpp"

struct PageGame : public Page, public Object
{
    void start() override;
    void update() override;
    void enter() override;
    void exit() override;

private:
    Snack *snack_;
    Food *food_;
    LED *led_;
};
