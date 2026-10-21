#pragma once

#include "main.h"
#include "object.hpp"

enum class ButtonState : uint8_t
{
    ON,
    OFF
};

enum class ButtonEvent : uint8_t
{
    OFF,
    PRESS,
    HOLD,
    RELEASE
};

struct Button : public Object
{
    Button(GPIO_TypeDef *GPIOX, uint16_t GPIO_Pin);
    void start() override;
    void update() override;

    ButtonState state{ButtonState::OFF};
    ButtonEvent event{ButtonEvent::OFF};
private:
    void listen();
    GPIO_TypeDef *GPIOX_;
    uint16_t GPIO_Pin_;
};