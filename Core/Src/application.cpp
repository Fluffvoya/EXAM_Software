#include "application.hpp"
#include "driver/OLED.hpp"

#include "object/button.hpp"
#include "object/LED.hpp"
#include "object/menu.hpp"
#include "object/image.hpp"
#include "object/anime.hpp"

#include "pages/page_led.hpp"
#include "pages/page_image.hpp"
#include "pages/page_anime.hpp"
#include "pages/page_game.hpp"

#include "image/club_logo.hpp"
#include "image/ui_elements.hpp"
#include "anime/aemeath_anime.hpp"

#include "startup.hpp"
#include "object_list.hpp"

class Application
{
public:
    static PageLED page_led;
    static PageImage page_image;
    static PageAnime page_anime;
    static PageGame page_game;

public:
    static void start();
    static void update();
};

PageLED Application::page_led{};
PageImage Application::page_image{};
PageAnime Application::page_anime{};
PageGame Application::page_game{};

Object *objects[] = {
    &ObjectList::button_back,
    &ObjectList::button_prev,
    &ObjectList::button_next,
    &ObjectList::button_ok,
    &ObjectList::button_left,
    &ObjectList::button_up,
    &ObjectList::button_down,
    &ObjectList::button_ok,
    &ObjectList::led,
    &ObjectList::image,
    &ObjectList::anime,
    &ObjectList::menu};

void Application::start()
{
    OLED::Init();
    startup();
    for (uint8_t i = 0; i < sizeof(objects) / sizeof(void *); i++)
        objects[i]->start();
    ObjectList::menu.pages[0] = &Application::page_led;
    ObjectList::menu.pages[1] = &Application::page_image;
    ObjectList::menu.pages[2] = &Application::page_anime;
    ObjectList::menu.pages[3] = &Application::page_game;
}

#define BUTTON_EVENT(BUTTON, EVENT, ACTION)

void Application::update()
{
    OLED::Clear();
    if (ObjectList::button_back.event == ButtonEvent::RELEASE)
        ObjectList::menu.exitPage();
    if (ObjectList::button_prev.event == ButtonEvent::RELEASE)
        ObjectList::menu.prevPage();
    if (ObjectList::button_next.event == ButtonEvent::RELEASE)
        ObjectList::menu.nextPage();
    if (ObjectList::button_ok.event == ButtonEvent::RELEASE)
        ObjectList::menu.enterPage();

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