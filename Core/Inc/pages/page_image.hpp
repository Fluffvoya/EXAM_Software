#pragma once

#include "pages/page.hpp"
#include "object/image.hpp"

struct PageImage : public Page
{
    PageImage(Image *image);
    void enter() override;
    void exit() override;

private:
    Image *image_{nullptr};
};
