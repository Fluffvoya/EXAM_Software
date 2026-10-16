#include "anime_demo.h"
#include "aemeath_anime.h"
#include "anime.h"

void EnterAnime() {
  showAnime(video_frames, VIDEO_FRAMES_FRAME_COUNT, 10, VIDEO_FRAMES_WIDTH,
            VIDEO_FRAMES_PAGES, 32, 0);
}

void ExitAnime() {
  OLED_Clear();
  OLED_Refresh();
}