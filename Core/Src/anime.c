#include "anime.h"
#include "OLED.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

void showAnime(const uint8_t *frames, uint8_t frame_num, uint8_t fps, int X,
               int Page, int POSX, int POSPage, uint8_t frame_state) {
  OLED_Clear();
  OLED_DrawBitmap(frames + frame_state * X * Page, X, Page, POSX, POSPage);
  HAL_Delay(1000 / fps);
}