#include "anime_demo.h"
#include "aemeath_anime.h"
#include "anime.h"
#include "menu.h"
#include <stdint.h>

uint8_t anime_menu_state;

void EnterAnime(Anime *anime) {
  anime_menu_state = 1;
  anime->frame_state = 0;
}

void UpdateAnime(Anime *anime) {
  if (anime_menu_state) {
    showAnime(anime);
    anime->frame_state += 1;
    anime->frame_state %= 50;
  }
}

void ExitAnime(Anime *anime) {
  anime_menu_state = 0;
  OLED_Clear();
  OLED_Refresh();
}