#include "pages/page_anime.hpp"
#include "image/ui_elements.hpp"
#include "object_list.hpp"

void PageAnime::start()
{
    bitmap_ = const_cast<uint8_t *>(text_anime);
    x_ = TEXT_ANIME_WIDTH;
    page_ = TEXT_PAGES;
    anime_ = &ObjectList::anime;
    active = false;
}

void PageAnime::update() {}

void PageAnime::enter()
{
    active = true;
    anime_->active = true;
}

void PageAnime::exit()
{
    active = false;
    anime_->active = false;
}