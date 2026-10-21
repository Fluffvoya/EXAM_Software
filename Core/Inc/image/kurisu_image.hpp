#include <stdint.h>

#define KURISU_IMAGE_WIDTH (64)
#define KURISU_IMAGE_HEIGHT (64)
#define KURISU_IMAGE_PAGES (8)
#define KURISU_IMAGE_STRIDE (512)     /* 每帧字节数 = PAGES * WIDTH */
#define KURISU_IMAGE_TOTAL_SIZE (512) /* 总字节数 = FRAME_COUNT * STRIDE */

extern const uint8_t kurisu_image[KURISU_IMAGE_TOTAL_SIZE];