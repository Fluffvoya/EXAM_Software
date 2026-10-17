#include "main.h"
#include "game_demo.h"
#include "OLED.h"
#include <stdint.h>
#include <sys/types.h>

#define DELTA_TIME 0.01

uint8_t game_menu_state = 0;

void EnterGame(Role *role) {
  game_menu_state = 1;
  OLED_Clear();
  OLED_Refresh();
}

static uint8_t IsTrigger(Role *role) {
  return ((int)(role->x) + 4) >= 120 && ((int)(role->x) + 4) <= 128 &&
         ((int)(role->y) + 4) >= 56 && ((int)(role->y) + 4) <= 64;
}

void UpdateGame(Role *role) {
  if (!game_menu_state)
    return;
  if (role->state == 1) {
    role->x -= DELTA_TIME * 75;
  }
  if (role->state == 2) {
    role->y -= DELTA_TIME * 75;
  }
  if (role->state == 3) {
    role->y += DELTA_TIME * 75;
  }
  if (role->state == 4) {
    role->x += DELTA_TIME * 75;
  }
  OLED_Clear();
  OLED_DrawRectangle((int)(role->x), (int)(role->y), 8, 8, 1, 1);
  if (!role->is_over && IsTrigger(role)) {
    role->is_over = 1;
  }
  if (!role->is_over)
    OLED_DrawCircle(124, 60, 2, 1, 1);
  else
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
}
void ExitGame(Role *role) {
  game_menu_state = 0;
  role->x = 0;
  role->y = 0;
  role->is_over = 0;
  role->state = 0;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
}
