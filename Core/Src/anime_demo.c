#include "anime_demo.h"
#include "aemeath_anime.h"
#include "anime.h"
#include <stdint.h>

uint8_t anime_menu_state;
uint8_t anime_frame_num = 0;

void EnterAnime() {
  anime_menu_state = 1;
  anime_frame_num = 0;
}

void UpdateAnime() {
  if (anime_menu_state) {
    showAnime(video_frames, VIDEO_FRAMES_FRAME_COUNT, 10, VIDEO_FRAMES_WIDTH,
              VIDEO_FRAMES_PAGES, 32, 0, anime_frame_num);
    anime_frame_num += 1;
    anime_frame_num %= 50;
  }
}

void ExitAnime() {
  anime_menu_state = 0;
  OLED_Clear();
  OLED_Refresh();
}