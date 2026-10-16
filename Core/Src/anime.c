#include "anime.h"
#include "OLED.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

void showAnime(const uint8_t *frames, uint8_t frame_num, uint8_t fps, int X,
               int Page, int POSX, int POSPage) {
  OLED_Clear();
  for (uint8_t index = 0; index < frame_num; index++) {
    OLED_DrawBitmap(frames + index * X * Page, X, Page, POSX, POSPage);
    OLED_Refresh();
    HAL_Delay(1000 / fps);
  }
}