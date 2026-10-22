#include "pages/page_image.hpp"
#include "image/ui_elements.hpp"
#include "object_list.hpp"

void PageImage::start()
{
    bitmap_ = const_cast<uint8_t *>(text_image);
    x_ = TEXT_IMAGE_WIDTH;
    page_ = TEXT_PAGES;
    image_ = &ObjectList::image;
    active = false;
}

void PageImage::update() {}

void PageImage::enter()
{
    active = true;
    image_->active = true;
}

void PageImage::exit()
{
    active = true;
    image_->active = false;
}