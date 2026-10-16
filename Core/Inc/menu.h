#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>

extern uint8_t menu_state;

void MenuNext();
void MenuPrev();

void EnterItem();
void ExitItem();

void ShowMenu();

void ButtonBackCallback();
void ButtonPrevCallback();
void ButtonNextCallback();
void ButtonOkCallback();
#endif
