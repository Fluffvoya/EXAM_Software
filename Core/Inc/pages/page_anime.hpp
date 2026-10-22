#pragma once

#include "pages/page.hpp"
#include "object/anime.hpp"

struct PageAnime : public Page, public Object
{
    void start() override;
    void update() override;
    void enter() override;
    void exit() override;

private:
    Anime *anime_{};
};
