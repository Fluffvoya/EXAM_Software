#include "pages/page_image.hpp"
#include "image/ui_elements.hpp"

PageImage::PageImage(Image *image)
    : image_(image)
{
    bitmap_ = const_cast<uint8_t *>(text_image);
    x_ = TEXT_IMAGE_WIDTH;
    page_ = TEXT_PAGES;
}

void PageImage::enter()
{
    image_->hide = false;
}

void PageImage::exit()
{
    image_->hide = true;
}