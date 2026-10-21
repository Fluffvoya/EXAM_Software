#include "pages/page_anime.hpp"
#include "image/ui_elements.hpp"

PageAnime::PageAnime(Anime *anime)
    : anime_(anime)
{
    bitmap_ = const_cast<uint8_t *>(text_anime);
    x_ = TEXT_ANIME_WIDTH;
    page_ = TEXT_PAGES;
}

void PageAnime::enter()
{
    anime_->hide = false;
}

void PageAnime::exit()
{
    anime_->hide = true;
}