#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>

extern uint8_t menu_state;

void ShowMenu();

void ButtonBackCallback(void *data);
void ButtonPrevCallback(void *data);
void ButtonNextCallback(void *data);
void ButtonOkCallback(void *data);
#endif
