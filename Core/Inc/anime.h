#ifndef __ANIME_H
#define __ANIME_H

#include "OLED.h"
#include <stdint.h>

typedef struct {
 uint8_t *frames;
  uint8_t frame_count, frame_state, fps ;
  int X, Page, POSX, POSPage;

} Anime;

void showAnime(Anime*anime);

#endif