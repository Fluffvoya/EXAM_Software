#include "application.hpp"
#include "driver/OLED.hpp"

#include "object/button.hpp"
#include "object/LED.hpp"
#include "object/menu.hpp"
#include "object/anime.hpp"

#include "pages/page_led.hpp"
#include "pages/page_image.hpp"
#include "pages/page_anime.hpp"
#include "pages/page_game.hpp"

#include "image/club_logo.hpp"
#include "image/ui_elements.hpp"
#include "anime/aemeath_anime.hpp"

#include "startup.hpp"

class Application
{
public:
    static Button button_back;
    static Button button_prev;
    static Button button_next;
    static Button button_ok;

    static Button button_left;
    static Button button_up;
    static Button button_down;
    static Button button_right;

    static LED led;
    static Image image;
    static Anime anime;

    static Menu<4> menu;
    static PageLED page_led;
    static PageImage page_image;
    static PageAnime page_anime;
    static PageGame page_game;

public:
    static void start();
    static void update();
};

Button Application::button_back{GPIOA, GPIO_PIN_7};
Button Application::button_prev{GPIOA, GPIO_PIN_6};
Button Application::button_next{GPIOA, GPIO_PIN_5};
Button Application::button_ok{GPIOA, GPIO_PIN_4};
Button Application::button_left{GPIOA, GPIO_PIN_3};
Button Application::button_up{GPIOA, GPIO_PIN_2};
Button Application::button_down{GPIOA, GPIO_PIN_1};
Button Application::button_right{GPIOA, GPIO_PIN_0};

LED Application::led{GPIOB, GPIO_PIN_11};
Image Application::image{
    const_cast<uint8_t *>(club_logo),
    CLUB_LOGO_WIDTH,
    CLUB_LOGO_PAGES,
    (OLED::WIDTH - CLUB_LOGO_WIDTH) / 2,
    (OLED::PAGES - CLUB_LOGO_PAGES) / 2};

Anime Application::anime{
    const_cast<uint8_t *>(video_frames),
    VIDEO_FRAMES_FRAME_COUNT,
    10,
    VIDEO_FRAMES_WIDTH,
    VIDEO_FRAMES_PAGES,
    (OLED::WIDTH - VIDEO_FRAMES_WIDTH) / 2,
    (OLED::PAGES - VIDEO_FRAMES_PAGES) / 2};

Menu<4> Application::menu{};

PageLED Application::page_led{&Application::led};
PageImage Application::page_image{&Application::image};
PageAnime Application::page_anime{&Application::anime};
PageGame Application::page_game{};

Object *objects[] = {
    &Application::button_back,
    &Application::button_prev,
    &Application::button_next,
    &Application::button_ok,
    &Application::button_left,
    &Application::button_up,
    &Application::button_down,
    &Application::button_right,

    &Application::led,
    &Application::image,
    &Application::anime,

    &Application::menu,
};

void Application::start()
{
    OLED::Init();
    startup();
    for (uint8_t i = 0; i < sizeof(objects) / sizeof(void *); i++)
        objects[i]->start();
    Application::menu.pages[0] = &Application::page_led;
    Application::menu.pages[1] = &Application::page_image;
    Application::menu.pages[2] = &Application::page_anime;
    Application::menu.pages[3] = &Application::page_game;
}

#define BUTTON_EVENT(BUTTON, EVENT, ACTION) \
    if (BUTTON.event == EVENT)              \
        ACTION;

void Application::update()
{
    OLED::Clear();
    BUTTON_EVENT(button_back, ButtonEvent::RELEASE, menu.exitPage())
    BUTTON_EVENT(button_prev, ButtonEvent::RELEASE, menu.prevPage())
    BUTTON_EVENT(button_next, ButtonEvent::RELEASE, menu.nextPage())
    BUTTON_EVENT(button_ok, ButtonEvent::RELEASE, menu.enterPage())

    for (uint8_t i = 0; i < sizeof(objects) / sizeof(void *); i++)
        objects[i]->update();
    OLED::Refresh();
    HAL_Delay(10);
}

void start()
{
    Application::start();
}

void update()
{
    Application::update();
}