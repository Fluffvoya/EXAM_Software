#pragma once

#include "pages/page.hpp"
#include "object/anime.hpp"

struct PageAnime : public Page
{
    PageAnime(Anime *anime);
    void enter() override;
    void exit() override;

private:
    Anime *anime_;
};
