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
    static void start();
    static void update();
};

Object *objects[] = {
    &ObjectList::button_back,
    &ObjectList::button_prev,
    &ObjectList::button_next,
    &ObjectList::button_ok,
    &ObjectList::button_left,
    &ObjectList::button_up,
    &ObjectList::button_down,
    &ObjectList::button_right,
    &ObjectList::led,
    &ObjectList::image,
    &ObjectList::anime,
    &ObjectList::snack,
    &ObjectList::food,
    &ObjectList::menu,
    &ObjectList::page_led,
    &ObjectList::page_image,
    &ObjectList::page_anime,
    &ObjectList::page_game,
};

void Application::start()
{
    OLED::Init();
    startup();
    for (uint8_t i = 0; i < sizeof(objects) / sizeof(void *); i++)
        objects[i]->start();
    ObjectList::menu.pages[0] = &ObjectList::page_led;
    ObjectList::menu.pages[1] = &ObjectList::page_image;
    ObjectList::menu.pages[2] = &ObjectList::page_anime;
    ObjectList::menu.pages[3] = &ObjectList::page_game;
}

#define BUTTON_EVENT(BUTTON, EVENT, ACTION) \
    if (BUTTON.event == EVENT)              \
        ACTION;

void Application::update()
{
    OLED::Clear();
    BUTTON_EVENT(ObjectList::button_back, ButtonEvent::RELEASE, ObjectList::menu.exitPage())
    BUTTON_EVENT(ObjectList::button_prev, ButtonEvent::RELEASE, ObjectList::menu.prevPage())
    BUTTON_EVENT(ObjectList::button_next, ButtonEvent::RELEASE, ObjectList::menu.nextPage())
    BUTTON_EVENT(ObjectList::button_ok, ButtonEvent::RELEASE, ObjectList::menu.enterPage())

    BUTTON_EVENT(ObjectList::button_left, ButtonEvent::PRESS, ObjectList::snack.turnLeft())
    BUTTON_EVENT(ObjectList::button_up, ButtonEvent::PRESS, ObjectList::snack.turnUp())
    BUTTON_EVENT(ObjectList::button_down, ButtonEvent::PRESS, ObjectList::snack.turnDown())
    BUTTON_EVENT(ObjectList::button_right, ButtonEvent::PRESS, ObjectList::snack.turnRight())

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