#include "button.h"

void ListenButton(Button *button) {
  GPIO_PinState state = HAL_GPIO_ReadPin(button->GPIOx, button->GPIO_Pin);
  // 按钮按下了
  if (state == GPIO_PIN_RESET) {
    // 第一次按下
    if (button->state == BUTTON_OFF) {
      button->state = BUTTON_ON;
      button->event = BUTTON_EVENT_PRESS;
    }
    // 之前就按下了
    else {
      button->event = BUTTON_EVENT_HOLD;
    }
  }
  // 按钮未按下
  else {
    // 上一帧也未按下
    if (button->state == BUTTON_OFF) {
      button->event = BUTTON_EVENT_OFF;
    }
    // 上一帧按下了
    else {
      button->state = BUTTON_OFF;
      button->event = BUTTON_EVENT_RELEASE;
    }
  }
}