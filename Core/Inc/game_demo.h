#ifndef __GAME_DEMO_H
#define __GAME_DEMO_H

#include <stdint.h>

typedef struct {
  float x, y;
  uint8_t is_over;
  uint8_t state;
} Role;

void EnterGame(Role *role);
void UpdateGame(Role *role);
void ExitGame(Role *role);

#endif