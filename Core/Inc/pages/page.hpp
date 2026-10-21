#pragma once

struct Page
{
    template <unsigned char page_num>
    friend class Menu;
    virtual void enter() = 0;
    virtual void exit() = 0;

protected:
    unsigned char *bitmap_{nullptr};
    int x_{};
    int page_{2};
};
