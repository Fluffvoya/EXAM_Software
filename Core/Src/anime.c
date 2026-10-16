#include "anime.h"
#include "OLED.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

void showAnime(Anime *anime) {
  OLED_Clear();
  OLED_DrawBitmap(anime->frames + anime->frame_state * anime->X * anime->Page,
                  anime->X, anime->Page, anime->POSX, anime->POSPage);
  HAL_Delay(1000 / anime->fps - 10);
}