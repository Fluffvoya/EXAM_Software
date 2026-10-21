#ifndef OLED_H
#define OLED_H

#include <stdint.h>

class OLED
{
public:
    /* 屏幕尺寸常量 */
    enum : uint8_t
    {
        WIDTH = 128, // 列数
        HEIGHT = 64, // 行数
        PAGES = 8    // 页数 = HEIGHT / 8
    };

    /* 纯静态工具类，禁止实例化 */
    OLED() = delete;
    OLED(const OLED &) = delete;
    OLED &operator=(const OLED &) = delete;

    /* 底层 I2C 访问 */
    static void WriteCommand(uint8_t cmd);
    static void WriteData(uint8_t *data, uint16_t size);
    static void SetCursor(uint8_t page, uint8_t x);

    /* 初始化与清屏 */
    static void Init(void);
    static void Clear(void);    // 全黑
    static void DisClear(void); // 全亮
    static void ClearSpace(int xStart, int xEnd, int yStart, int yEnd);
    static void Refresh(void); // 把显存刷到屏幕

    /* 绘图 */
    static void DrawBitmap(unsigned char bitmap[], int x, int page,
                           int posX, int posPage);
    static void DrawPixel(int x, int y, uint8_t color);
    static void DrawRectangle(int x, int y, int width, int height,
                              uint8_t filled, uint8_t color);
    static void DrawCircle(int x0, int y0, int r,
                           uint8_t filled, uint8_t color);

private:
    static uint8_t s_gram[PAGES][WIDTH]; // 8 页 x 128 列 显存
};

#endif // OLED_H