#include "driver/OLED.hpp"
#include "main.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

namespace
{
  constexpr uint8_t kI2CAddr = 0x78; // 0x3C << 1

  /* 用函数返回句柄，避免全局引用初始化顺序问题 */
  inline I2C_HandleTypeDef *i2cHandle() { return &hi2c1; }
} // namespace

/* 静态成员定义 */
uint8_t OLED::s_gram[OLED::PAGES][OLED::WIDTH];

/* ------------------------- 底层 I2C 访问 ------------------------- */
void OLED::WriteCommand(uint8_t cmd)
{
  HAL_I2C_Mem_Write(i2cHandle(), kI2CAddr,
                    0x00, // 控制字节：命令
                    I2C_MEMADD_SIZE_8BIT, &cmd, 1, HAL_MAX_DELAY);
}

void OLED::WriteData(uint8_t *data, uint16_t size)
{
  HAL_I2C_Mem_Write(i2cHandle(), kI2CAddr,
                    0x40, // 控制字节：数据
                    I2C_MEMADD_SIZE_8BIT, data, size, HAL_MAX_DELAY);
}

void OLED::SetCursor(uint8_t page, uint8_t x)
{
  WriteCommand(0xB0 | page);              // 页地址 0~7
  WriteCommand(0x10 | ((x & 0xF0) >> 4)); // 列地址高 4 位
  WriteCommand(0x00 | (x & 0x0F));        // 列地址低 4 位
}

/* ------------------------- 初始化与清屏 ------------------------- */
void OLED::Init(void)
{
  /* I2C 由 MX_I2C1_Init() 初始化，这里不再调用 IIC_Init() */
  HAL_Delay(100);

  WriteCommand(0xAE); // 关闭显示

  WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
  WriteCommand(0x80);

  WriteCommand(0xA8); // 设置多路复用率
  WriteCommand(0x3F);

  WriteCommand(0xD3); // 设置显示偏移
  WriteCommand(0x00);

  WriteCommand(0x40); // 设置显示开始行

  WriteCommand(0xA1); // 左右方向：0xA1 正常
  WriteCommand(0xC8); // 上下方向：0xC8 正常

  WriteCommand(0xDA); // 设置 COM 引脚硬件配置
  WriteCommand(0x12);

  WriteCommand(0x81); // 设置对比度控制
  WriteCommand(0xCF);

  WriteCommand(0xD9); // 设置预充电周期
  WriteCommand(0xF1);

  WriteCommand(0xDB); // 设置 VCOMH 取消选择级别
  WriteCommand(0x30);

  WriteCommand(0xA4); // 设置整个显示打开/关闭
  WriteCommand(0xA6); // 设置正常/倒转显示

  WriteCommand(0x8D); // 设置充电泵
  WriteCommand(0x14);

  WriteCommand(0xAF); // 开启显示

  Clear();
  Refresh();
}

void OLED::Clear(void)
{
  memset(s_gram, 0x00, sizeof(s_gram));
}

void OLED::DisClear(void)
{
  memset(s_gram, 0xFF, sizeof(s_gram));
}

void OLED::ClearSpace(int xStart, int xEnd, int yStart, int yEnd)
{
  if (xStart < 0)
    xStart = 0;
  if (xEnd > 127)
    xEnd = 127;
  if (yStart < 0)
    yStart = 0;
  if (yEnd > 7)
    yEnd = 7;

  if (xStart > xEnd || yStart > yEnd)
  {
    return;
  }

  for (int j = yStart; j <= yEnd; j++)
  {
    for (int i = xStart; i <= xEnd; i++)
    {
      s_gram[j][i] = 0x00;
    }
  }
}

void OLED::Refresh(void)
{
  for (uint8_t page = 0; page < PAGES; page++)
  {
    SetCursor(page, 0);
    WriteData(&s_gram[page][0], WIDTH);
  }
}

/* ----------------------------- 绘图 ----------------------------- */
void OLED::DrawBitmap(unsigned char bitmap[], int x, int page,
                      int posX, int posPage)
{
  int index = 0;

  for (int j = 0; j < page; j++)
  {
    int pg = posPage + j;

    for (int i = 0; i < x; i++)
    {
      int col = posX + i;

      if (pg >= 0 && pg < PAGES && col >= 0 && col < WIDTH)
      {
        s_gram[pg][col] = bitmap[index];
      }

      index++;
    }
  }
}

// 画点：color=1 点亮，color=0 擦除
void OLED::DrawPixel(int x, int y, uint8_t color)
{
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
  {
    return;
  }

  if (color)
  {
    s_gram[y / 8][x] |= (1 << (y % 8));
  }
  else
  {
    s_gram[y / 8][x] &= ~(1 << (y % 8));
  }
}

// 画矩形：x,y 左上角，width 宽，height 高（单位像素）
// filled=0 空心，filled=1 填充；color=1 点亮，color=0 擦除
void OLED::DrawRectangle(int x, int y, int width, int height,
                         uint8_t filled, uint8_t color)
{
  if (width <= 0 || height <= 0)
  {
    return;
  }

  int x0 = x;
  int y0 = y;
  int x1 = x + width - 1;
  int y1 = y + height - 1;

  // 裁剪到屏幕范围
  if (x0 < 0)
    x0 = 0;
  if (y0 < 0)
    y0 = 0;
  if (x1 > WIDTH - 1)
    x1 = WIDTH - 1;
  if (y1 > HEIGHT - 1)
    y1 = HEIGHT - 1;

  if (x0 > x1 || y0 > y1)
  {
    return;
  }

  if (filled)
  {
    for (int yy = y0; yy <= y1; yy++)
    {
      for (int xx = x0; xx <= x1; xx++)
      {
        DrawPixel(xx, yy, color);
      }
    }
  }
  else
  {
    for (int xx = x0; xx <= x1; xx++)
    {
      DrawPixel(xx, y0, color);
      DrawPixel(xx, y1, color);
    }
    for (int yy = y0; yy <= y1; yy++)
    {
      DrawPixel(x0, yy, color);
      DrawPixel(x1, yy, color);
    }
  }
}

// 画圆：filled=0 空心（中点画圆），filled=1 填充（逐行扫描）
void OLED::DrawCircle(int x0, int y0, int r, uint8_t filled, uint8_t color)
{
  if (r <= 0)
  {
    return;
  }

  if (filled)
  {
    for (int dy = -r; dy <= r; dy++)
    {
      // 整数逼近 dx = sqrt(r^2 - dy^2)
      int dx = 0;
      while ((dx + 1) * (dx + 1) + dy * dy <= r * r)
      {
        dx++;
      }

      int xStart = x0 - dx;
      int xEnd = x0 + dx;

      for (int xx = xStart; xx <= xEnd; xx++)
      {
        DrawPixel(xx, y0 + dy, color);
      }
    }
  }
  else
  {
    int x = 0;
    int y = r;
    int d = 1 - r;

    while (x <= y)
    {
      DrawPixel(x0 + x, y0 + y, color);
      DrawPixel(x0 - x, y0 + y, color);
      DrawPixel(x0 + x, y0 - y, color);
      DrawPixel(x0 - x, y0 - y, color);

      DrawPixel(x0 + y, y0 + x, color);
      DrawPixel(x0 - y, y0 + x, color);
      DrawPixel(x0 + y, y0 - x, color);
      DrawPixel(x0 - y, y0 - x, color);

      if (d < 0)
      {
        d += 2 * x + 3;
      }
      else
      {
        d += 2 * (x - y) + 5;
        y--;
      }
      x++;
    }
  }
}