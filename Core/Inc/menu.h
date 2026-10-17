#ifndef __MENU_H
#define __MENU_H

#include "data.h"
#include <stdint.h>

extern uint8_t menu_state;

void ShowMenu();

void ButtonBackCallback(Data *data);
void ButtonPrevCallback(Data *data);
void ButtonNextCallback(Data *data);
void ButtonOkCallback(Data *data);

void ButtonLeftCallback(Data *data);
void ButtonUpCallback(Data *data);
void ButtonDownCallback(Data *data);
void ButtonRightCallback(Data *data);

void ButtonReleaseCallback(Data *data);
#endif
