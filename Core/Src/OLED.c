#include "OLED.h"
#include "main.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define IICAddr 0x78
#define OLED_I2C_HANDLE hi2c1

// 128x64 OLED，8 页，每页 128 字节
static uint8_t OLED_GRAM[8][128];

void OLED_WriteCommand(uint8_t Cmd) {
  HAL_I2C_Mem_Write(&OLED_I2C_HANDLE, IICAddr,
                    0x00, // 控制字节：命令
                    I2C_MEMADD_SIZE_8BIT, &Cmd, 1, HAL_MAX_DELAY);
}

void OLED_WriteData(uint8_t *data, uint16_t size) {
  HAL_I2C_Mem_Write(&OLED_I2C_HANDLE, IICAddr,
                    0x40, // 控制字节：数据
                    I2C_MEMADD_SIZE_8BIT, data, size, HAL_MAX_DELAY);
}

void OLED_SetCursor(uint8_t Y, uint8_t X) {
  OLED_WriteCommand(0xB0 | Y);                 // 设置页地址 Y：0~7
  OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // 列地址高 4 位
  OLED_WriteCommand(0x00 | (X & 0x0F));        // 列地址低 4 位
}

void OLED_Clear(void) { memset(OLED_GRAM, 0x00, sizeof(OLED_GRAM)); }

void OLED_ClearSpace(int X_START, int X_END, int Y_START, int Y_END) {
  if (X_START < 0)
    X_START = 0;
  if (X_END > 127)
    X_END = 127;
  if (Y_START < 0)
    Y_START = 0;
  if (Y_END > 7)
    Y_END = 7;

  if (X_START > X_END || Y_START > Y_END) {
    return;
  }

  for (int j = Y_START; j <= Y_END; j++) {
    for (int i = X_START; i <= X_END; i++) {
      OLED_GRAM[j][i] = 0x00;
    }
  }
}

void OLED_DisClear(void) { memset(OLED_GRAM, 0xFF, sizeof(OLED_GRAM)); }

void OLED_Init(void) {
  // HAL 库中 I2C 应由 MX_I2Cx_Init() 初始化，这里不再调用 IIC_Init()
  HAL_Delay(100);

  OLED_WriteCommand(0xAE); // 关闭显示

  OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
  OLED_WriteCommand(0x80);

  OLED_WriteCommand(0xA8); // 设置多路复用率
  OLED_WriteCommand(0x3F);

  OLED_WriteCommand(0xD3); // 设置显示偏移
  OLED_WriteCommand(0x00);

  OLED_WriteCommand(0x40); // 设置显示开始行

  OLED_WriteCommand(0xA1); // 左右方向，0xA1 正常
  OLED_WriteCommand(0xC8); // 上下方向，0xC8 正常

  OLED_WriteCommand(0xDA); // 设置 COM 引脚硬件配置
  OLED_WriteCommand(0x12);

  OLED_WriteCommand(0x81); // 设置对比度控制
  OLED_WriteCommand(0xCF);

  OLED_WriteCommand(0xD9); // 设置预充电周期
  OLED_WriteCommand(0xF1);

  OLED_WriteCommand(0xDB); // 设置 VCOMH 取消选择级别
  OLED_WriteCommand(0x30);

  OLED_WriteCommand(0xA4); // 设置整个显示打开/关闭
  OLED_WriteCommand(0xA6); // 设置正常/倒转显示

  OLED_WriteCommand(0x8D); // 设置充电泵
  OLED_WriteCommand(0x14);

  OLED_WriteCommand(0xAF); // 开启显示

  OLED_Clear();
  OLED_Refresh();
}

void OLED_DrawBitmap(const unsigned char Bitmap[], int X, int Page, int POSX,
                     int POSPage) {
  int Index = 0;

  for (int j = 0; j < Page; j++) {
    int page = POSPage + j;

    for (int i = 0; i < X; i++) {
      int col = POSX + i;

      if (page >= 0 && page < 8 && col >= 0 && col < 128) {
        OLED_GRAM[page][col] = Bitmap[Index];
      }

      Index++;
    }
  }
}

void OLED_Refresh(void) {
  for (uint8_t page = 0; page < 8; page++) {
    OLED_SetCursor(page, 0);
    OLED_WriteData(&OLED_GRAM[page][0], 128);
  }
}
