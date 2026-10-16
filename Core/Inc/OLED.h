#ifndef OLED_H
#define OLED_H

#ifndef __OLED
#define __OLED

#include <stdint.h>

void OLED_WriteCommand(uint8_t Cmd);
void OLED_WriteData(uint8_t *data, uint16_t size);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
void OLED_ClearSpace(int X_START, int X_END, int Y_START, int Y_END);
void OLED_DisClear(void);
void OLED_Init(void);
void OLED_DrawBitmap(const unsigned char Bitmap[], int X, int Page, int POSX,
                     int POSPage);
void OLED_Refresh(void);
void OLED_DrawPixel(int X, int Y, uint8_t Color);
void OLED_DrawRectangle(int X, int Y, int Width, int Height, uint8_t Filled,
                        uint8_t Color);
void OLED_DrawCircle(int X0, int Y0, int R, uint8_t Filled, uint8_t Color);
#endif
#endif