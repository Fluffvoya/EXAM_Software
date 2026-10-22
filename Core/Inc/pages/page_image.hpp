#pragma once

#include "pages/page.hpp"
#include "object/image.hpp"

struct PageImage : public Page, public Object
{
    void start() override;
    void update() override;
    void enter() override;
    void exit() override;

private:
    Image *image_{};
};
