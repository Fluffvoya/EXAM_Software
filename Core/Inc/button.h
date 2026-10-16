#ifndef __BUTTON_H
#define __BUTTON_H

#include "main.h"
#include <stdint.h>

#define BUTTON_OFF 0
#define BUTTON_ON 1

#define BUTTON_EVENT_OFF 0
#define BUTTON_EVENT_PRESS 1
#define BUTTON_EVENT_HOLD 2
#define BUTTON_EVENT_RELEASE 3

typedef struct {
  GPIO_TypeDef *GPIOx;
  uint16_t GPIO_Pin;
  uint8_t state;
  uint8_t event;
} Button;

typedef void (*Callback)(void *data);

void ListenButton(Button *button);
void ListenEvent(Button *button, uint8_t event, Callback callback,void*data);

#endif