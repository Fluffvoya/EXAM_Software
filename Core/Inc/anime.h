#ifndef __ANIME_H
#define __ANIME_H

#include "OLED.h"
#include <stdint.h>

void showAnime(const uint8_t *frames, uint8_t frame_num, uint8_t fps, int X,
               int Page, int POSX, int POSPage, uint8_t frame_state);

#endif