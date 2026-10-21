#pragma once

#include "pages/page.hpp"

struct PageGame : public Page
{
    PageGame();
    void enter() override;
    void exit() override;
};
