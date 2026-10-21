/**
 * @file    aemeath_anime.h
 * @brief   由 video2oled.py 自动生成，请勿手动修改
 *
 * 源视频      : .\aemeath.mp4
 * 源分辨率    : 3840 x 3840 @ 60.00 fps
 * 提取帧率    : 10.0 fps
 * 帧数        : 50
 * 图像尺寸    : 64 x 64  (宽 x 高)
 * 取模方式    : 阴码 / 逐列式 / 逆向
 *   阴码   : 数据位 1 = 点亮像素
 *   逐列式 : 每字节 = 一列的 8 个像素
 *   逆向   : bit0 = 页内最上一行, bit7 = 页内最下一行
 *
 * 数组结构    : 一维数组（所有帧顺序拼接）
 *   name[frame * STRIDE + page * WIDTH + col]
 *   frame -> 第几帧
 *   page  -> 第几页 (0 ~ PAGES-1)
 *   col   -> 第几列 (0 ~ WIDTH-1)
 *
 * 显示参数    :
 *   X    (列) : 0 ~ 63   共 64 列
 *   Page (页) : 0 ~ 7   共 8 页
 *
 * 数据大小    : 50 x 512 = 25600 字节 (25.0 KB)
 *
 * ---------------- STM32 显示示例 ----------------
 *
 *   // 假设驱动已提供:
 *   //   void SSD1306_SetCursor(uint8_t col, uint8_t page);
 *   //   void SSD1306_WriteBytes(const uint8_t *buf, uint16_t len);
 *
 *   void OLED_ShowVideoFrame(uint16_t idx)
 *   {
 *       if (idx >= VIDEO_FRAMES_FRAME_COUNT) return;
 *
 *       const uint8_t *p = &video_frames[idx * VIDEO_FRAMES_STRIDE];
 *
 *       for (uint8_t page = 0; page < VIDEO_FRAMES_PAGES; page++) {
 *           SSD1306_SetCursor(0, page);       // X = 0, Page = page
 *           SSD1306_WriteBytes(&p[page * VIDEO_FRAMES_WIDTH], VIDEO_FRAMES_WIDTH);
 *       }
 *   }
 *
 *   // 播放循环 (10 fps -> 每帧 100 ms)
 *   // uint16_t f = 0;
 *   // while (1) {
 *   //     OLED_ShowVideoFrame(f);
 *   //     f = (f + 1) % VIDEO_FRAMES_FRAME_COUNT;
 *   //     HAL_Delay(100);
 *   // }
 */

#ifndef __VIDEO_FRAMES_H__
#define __VIDEO_FRAMES_H__

#include <stdint.h>

#define VIDEO_FRAMES_FRAME_COUNT (50)
#define VIDEO_FRAMES_WIDTH (64)
#define VIDEO_FRAMES_HEIGHT (64)
#define VIDEO_FRAMES_PAGES (8)
#define VIDEO_FRAMES_STRIDE (512)       /* 每帧字节数 = PAGES * WIDTH */
#define VIDEO_FRAMES_TOTAL_SIZE (25600) /* 总字节数 = FRAME_COUNT * STRIDE */

/* video_frames[frame * STRIDE + page * WIDTH + col] */
extern const uint8_t video_frames[VIDEO_FRAMES_TOTAL_SIZE];

#endif /* __VIDEO_FRAMES_H__ */
