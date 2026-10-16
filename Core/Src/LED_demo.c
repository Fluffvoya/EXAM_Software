#include "OLED.h"
#include "main.h"

void EnterLED() {
  OLED_DisClear();
  OLED_Refresh();
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
}

void ExitLED() { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); }