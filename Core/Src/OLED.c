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

void OLED_DrawBitmap(unsigned char Bitmap[], int X, int Page, int POSX,
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

// 画点：Color=1 点亮，Color=0 擦除
void OLED_DrawPixel(int X, int Y, uint8_t Color) {
  if (X < 0 || X >= 128 || Y < 0 || Y >= 64) {
    return;
  }

  if (Color) {
    OLED_GRAM[Y / 8][X] |= (1 << (Y % 8));
  } else {
    OLED_GRAM[Y / 8][X] &= ~(1 << (Y % 8));
  }
}

// 画矩形：X,Y 左上角，Width 宽，Height 高，单位都是像素
// Filled=0 空心矩形，Filled=1 填充矩形
// Color=1 点亮，Color=0 擦除
void OLED_DrawRectangle(int X, int Y, int Width, int Height, uint8_t Filled,
                        uint8_t Color) {
  if (Width <= 0 || Height <= 0) {
    return;
  }

  int x0 = X;
  int y0 = Y;
  int x1 = X + Width - 1;
  int y1 = Y + Height - 1;

  // 裁剪到屏幕范围
  if (x0 < 0)
    x0 = 0;
  if (y0 < 0)
    y0 = 0;
  if (x1 > 127)
    x1 = 127;
  if (y1 > 63)
    y1 = 63;

  if (x0 > x1 || y0 > y1) {
    return;
  }

  if (Filled) {
    // 填充矩形
    for (int y = y0; y <= y1; y++) {
      for (int x = x0; x <= x1; x++) {
        OLED_DrawPixel(x, y, Color);
      }
    }
  } else {
    // 空心矩形
    for (int x = x0; x <= x1; x++) {
      OLED_DrawPixel(x, y0, Color);
      OLED_DrawPixel(x, y1, Color);
    }

    for (int y = y0; y <= y1; y++) {
      OLED_DrawPixel(x0, y, Color);
      OLED_DrawPixel(x1, y, Color);
    }
  }
}

void OLED_DrawCircle(int X0, int Y0, int R, uint8_t Filled, uint8_t Color) {
  if (R <= 0) {
    return;
  }

  if (Filled) {
    // 填充圆：逐行扫描，画水平线
    for (int y = -R; y <= R; y++) {
      // 计算当前行对应的 x 偏移量
      // dx = sqrt(R^2 - y^2)
      int dx = 0;
      // 用整数逼近，避免浮点运算
      while ((dx + 1) * (dx + 1) + y * y <= R * R) {
        dx++;
      }

      int x_start = X0 - dx;
      int x_end = X0 + dx;

      for (int x = x_start; x <= x_end; x++) {
        OLED_DrawPixel(x, Y0 + y, Color);
      }
    }
  } else {
    // 空心圆：中点画圆算法
    int x = 0;
    int y = R;
    int d = 1 - R;

    while (x <= y) {
      OLED_DrawPixel(X0 + x, Y0 + y, Color);
      OLED_DrawPixel(X0 - x, Y0 + y, Color);
      OLED_DrawPixel(X0 + x, Y0 - y, Color);
      OLED_DrawPixel(X0 - x, Y0 - y, Color);

      OLED_DrawPixel(X0 + y, Y0 + x, Color);
      OLED_DrawPixel(X0 - y, Y0 + x, Color);
      OLED_DrawPixel(X0 + y, Y0 - x, Color);
      OLED_DrawPixel(X0 - y, Y0 - x, Color);

      if (d < 0) {
        d += 2 * x + 3;
      } else {
        d += 2 * (x - y) + 5;
        y--;
      }
      x++;
    }
  }
}

