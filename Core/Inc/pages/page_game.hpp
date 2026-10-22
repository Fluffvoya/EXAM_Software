#pragma once

#include "pages/page.hpp"
#include "object/snack.hpp"

struct PageGame : public Page, public Object
{
    void start() override;
    void update() override;
    void enter() override;
    void exit() override;

private:
    Snack *snack_;
};
