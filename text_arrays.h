/**
 * @file    text_arrays.h
 * @brief   由 text2oled.py 自动生成，请勿手动修改
 *
 * 字形        : 内置 8x16 ASCII 点阵（每字形高 16px = 2 Page）
 * 宽度        : 自适应（逐字形裁剪空白列后拼接）
 * 取模方式    : 阴码 / 逐列式 / 逆向
 *   阴码   : 数据位 1 = 点亮像素
 *   逐列式 : 每字节 = 一列的 8 个像素
 *   逆向   : bit0 = 页内最上一行, bit7 = 页内最下一行
 *
 * 数组结构    : name_xxx[page * WIDTH + col]
 *
 * 字符串一览  :
 *   text_LED                 X(列) = 22   Page = 2      44 字节   "LED"
 *   text_Image               X(列) = 37   Page = 2      74 字节   "Image"
 *   text_Anime               X(列) = 38   Page = 2      76 字节   "Anime"
 *   text_Game                X(列) = 30   Page = 2      60 字节   "Game"
 *
 * ---------------- STM32 显示示例 ----------------
 *
 *   void OLED_ShowText(const uint8_t *d, uint8_t pages, uint8_t w)
 *   {
 *       for (uint8_t p = 0; p < pages; p++) {
 *           SSD1306_SetCursor(0, p);
 *           SSD1306_WriteBytes(&d[p * w], w);
 *       }
 *   }
 *
 */

#ifndef __TEXT_H__
#define __TEXT_H__

#include <stdint.h>

#define TEXT_COUNT   (4)


/* -------------------------------------------------------------- */
/* "LED"   X(列) = 22   Page = 2   共 44 字节 */
#define TEXT_LED_X       (22)
#define TEXT_LED_PAGES   (2)
#define TEXT_LED_SIZE    (TEXT_LED_PAGES * TEXT_LED_X)

/* text_LED[page * TEXT_LED_X + col] */
static const 
/* -------------------------------------------------------------- */
/* "Image"   X(列) = 37   Page = 2   共 74 字节 */
#define TEXT_IMAGE_X       (37)
#define TEXT_IMAGE_PAGES   (2)
#define TEXT_IMAGE_SIZE    (TEXT_IMAGE_PAGES * TEXT_IMAGE_X)

/* text_Image[page * TEXT_IMAGE_X + col] */
static const 
/* -------------------------------------------------------------- */
/* "Anime"   X(列) = 38   Page = 2   共 76 字节 */
#define TEXT_ANIME_X       (38)
#define TEXT_ANIME_PAGES   (2)
#define TEXT_ANIME_SIZE    (TEXT_ANIME_PAGES * TEXT_ANIME_X)

/* text_Anime[page * TEXT_ANIME_X + col] */
static const 
/* -------------------------------------------------------------- */
/* "Game"   X(列) = 30   Page = 2   共 60 字节 */
#define TEXT_GAME_X       (30)
#define TEXT_GAME_PAGES   (2)
#define TEXT_GAME_SIZE    (TEXT_GAME_PAGES * TEXT_GAME_X)

/* text_Game[page * TEXT_GAME_X + col] */
static const 
#endif /* __TEXT_H__ */
