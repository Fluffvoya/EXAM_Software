#include "object_list.hpp"
#include "main.h"
#include "image/club_logo.hpp"
#include "image/ui_elements.hpp"
#include "anime/aemeath_anime.hpp"

Button ObjectList::button_back{GPIOA, GPIO_PIN_7};
Button ObjectList::button_prev{GPIOA, GPIO_PIN_6};
Button ObjectList::button_next{GPIOA, GPIO_PIN_5};
Button ObjectList::button_ok{GPIOA, GPIO_PIN_4};
Button ObjectList::button_left{GPIOA, GPIO_PIN_3};
Button ObjectList::button_up{GPIOA, GPIO_PIN_2};
Button ObjectList::button_down{GPIOA, GPIO_PIN_1};
Button ObjectList::button_right{GPIOA, GPIO_PIN_0};

LED ObjectList::led{GPIOB, GPIO_PIN_11};
Image ObjectList::image{
    const_cast<uint8_t *>(club_logo),
    CLUB_LOGO_WIDTH,
    CLUB_LOGO_PAGES,
    (OLED::WIDTH - CLUB_LOGO_WIDTH) / 2,
    (OLED::PAGES - CLUB_LOGO_PAGES) / 2};

Anime ObjectList::anime{
    const_cast<uint8_t *>(video_frames),
    VIDEO_FRAMES_FRAME_COUNT,
    10,
    VIDEO_FRAMES_WIDTH,
    VIDEO_FRAMES_PAGES,
    (OLED::WIDTH - VIDEO_FRAMES_WIDTH) / 2,
    (OLED::PAGES - VIDEO_FRAMES_PAGES) / 2};

Menu<4> ObjectList::menu{};