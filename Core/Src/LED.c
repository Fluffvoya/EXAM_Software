#include "main.h"

void EnterLED() { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); }

void ExitLED() { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); }