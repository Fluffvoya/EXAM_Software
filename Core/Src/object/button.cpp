#include "object/button.hpp"

Button::Button(GPIO_TypeDef *GPIOX, uint16_t GPIO_Pin)
    : GPIOX_(GPIOX), GPIO_Pin_(GPIO_Pin) {}

void Button::start()
{
}

void Button::update()
{
    if (!active)
        return;
    listen();
}

void Button::listen()
{
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(GPIOX_, GPIO_Pin_);
    // 按钮按下了
    if (pin_state == GPIO_PIN_RESET)
    {
        // 第一次按下
        if (state == ButtonState::OFF)
        {
            state = ButtonState::ON;
            event = ButtonEvent::PRESS;
        }
        // 之前就按下了
        else
        {
            event = ButtonEvent::HOLD;
        }
    }
    // 按钮未按下
    else
    {
        // 上一帧也未按下
        if (state == ButtonState::OFF)
        {
            event = ButtonEvent::OFF;
        }
        // 上一帧按下了
        else
        {
            state = ButtonState::OFF;
            event = ButtonEvent::RELEASE;
        }
    }
}