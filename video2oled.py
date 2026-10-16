#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
video2oled.py
================================================================
按指定帧率从视频中提取帧，缩放到高度 64、宽度自适应（<=128），
按「阴码 / 逐列式 / 逆向」取模，生成 SSD1306 OLED 用的 C 头文件。

输出数组为一维（所有帧顺序拼接）:
    name[frame * STRIDE + page * WIDTH + col]

依赖:
    pip install opencv-python numpy

示例:
    python video2oled.py bad_apple.mp4 -f 12 -o bad_apple.h -n bad_apple
    python video2oled.py test.mp4 -f 10 --max-frames 60 --invert -p preview.png
================================================================
"""

import argparse
import os
import sys

import numpy as np

try:
    import cv2
except ImportError:
    sys.exit("错误: 未安装 OpenCV，请执行  pip install opencv-python")


# --------------------------------------------------------------------------- #
# 命令行参数
# --------------------------------------------------------------------------- #
def parse_args():
    p = argparse.ArgumentParser(
        description="按帧率提取视频帧 -> 高度64/宽度自适应 -> SSD1306 列式阴码头文件",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    p.add_argument("input", help="输入视频文件路径")
    p.add_argument("-o", "--output", default="video_frames.h", help="输出头文件路径")
    p.add_argument("-n", "--name", default="video_frames", help="生成的 C 数组名")
    p.add_argument("-f", "--fps", type=float, default=10.0, help="每秒提取多少帧")
    p.add_argument("--height", type=int, default=64, help="目标高度(必须是 8 的倍数)")
    p.add_argument("--max-width", type=int, default=128, help="最大宽度(SSD1306 通常为 128)")
    p.add_argument("--max-frames", type=int, default=0, help="最多提取多少帧, 0 表示不限")
    p.add_argument("--threshold", type=int, default=128, help="二值化阈值 0~255")
    p.add_argument("--no-dither", action="store_true", help="关闭 Floyd-Steinberg 抖动, 用硬阈值")
    p.add_argument("--invert", action="store_true", help="反色(相当于阴码 <-> 阳码)")
    p.add_argument("--clahe", action="store_true", help="启用自适应直方图均衡, 提升对比度")
    p.add_argument("-p", "--preview", default="", help="把第一帧保存为 PNG 预览(调试用)")
    return p.parse_args()


# --------------------------------------------------------------------------- #
# 图像处理
# --------------------------------------------------------------------------- #
def resize_frame(gray, target_h, max_w):
    """等比缩放到高度 target_h；若宽度超过 max_w，则改按宽度缩放并垂直居中。"""
    h, w = gray.shape[:2]
    if h == 0 or w == 0:
        return None

    new_w = max(1, int(round(w * target_h / h)))

    if new_w <= max_w:
        return cv2.resize(gray, (new_w, target_h), interpolation=cv2.INTER_AREA)

    # 宽度超限：按宽度缩放，高度可能 < target_h，上下补黑居中
    scaled_h = min(target_h, max(1, int(round(h * max_w / w))))
    resized = cv2.resize(gray, (max_w, scaled_h), interpolation=cv2.INTER_AREA)
    canvas = np.zeros((target_h, max_w), dtype=np.uint8)
    y0 = (target_h - scaled_h) // 2
    canvas[y0:y0 + scaled_h, :] = resized
    return canvas


def binarize(gray, threshold=128, dither=True, invert=False):
    """灰度 -> 0/1 二值图。1 表示点亮（阴码）。"""
    img = gray.astype(np.float32)
    h, w = img.shape

    if dither:
        out = np.zeros((h, w), dtype=np.uint8)
        for y in range(h):
            for x in range(w):
                old = img[y, x]
                new = 255.0 if old > threshold else 0.0
                out[y, x] = 1 if new > 0.0 else 0
                err = old - new
                if x + 1 < w:
                    img[y, x + 1] += err * 7.0 / 16.0
                if y + 1 < h:
                    if x > 0:
                        img[y + 1, x - 1] += err * 3.0 / 16.0
                    img[y + 1, x] += err * 5.0 / 16.0
                    if x + 1 < w:
                        img[y + 1, x + 1] += err * 1.0 / 16.0
    else:
        out = (img > threshold).astype(np.uint8)

    if invert:
        out = 1 - out
    return out


def pack_column_major(binary, height):
    """
    阴码 / 逐列式 / 逆向 取模。
      - 逐列式: 每字节 = 一列的 8 个像素（同一 page 内）
      - 逆向  : bit0 = 页内最上一行, bit7 = 页内最下一行
    返回 shape = (pages, width) 的 uint8 数组。
    """
    h, w = binary.shape
    pages = height // 8
    data = np.zeros((pages, w), dtype=np.uint8)

    for page in range(pages):
        for bit in range(8):
            y = page * 8 + bit
            if y >= h:
                break
            row = binary[y].astype(np.uint8)          # 0/1
            data[page] |= np.left_shift(row, bit).astype(np.uint8)

    return data


# --------------------------------------------------------------------------- #
# 生成头文件
# --------------------------------------------------------------------------- #
def write_header(path, name, packed_frames, width, height, meta):
    """
    packed_frames : list of np.ndarray, 每项 shape = (pages, width)
    输出一维数组: name[frame * STRIDE + page * width + col]
    """
    pages = height // 8
    count = len(packed_frames)
    uname = name.upper()
    guard = f"__{uname}_H__"
    stride = pages * width                       # 一帧的字节数
    total = count * stride

    # 把所有帧顺序拼接成一个一维数组
    flat_all = np.concatenate([f.reshape(-1) for f in packed_frames]) \
        if packed_frames else np.zeros(0, dtype=np.uint8)

    L = []
    L.append("/**")
    L.append(f" * @file    {os.path.basename(path)}")
    L.append(" * @brief   由 video2oled.py 自动生成，请勿手动修改")
    L.append(" *")
    L.append(f" * 源视频      : {meta['src']}")
    L.append(f" * 源分辨率    : {meta['sw']} x {meta['sh']} @ {meta['sfps']:.2f} fps")
    L.append(f" * 提取帧率    : {meta['fps']} fps")
    L.append(f" * 帧数        : {count}")
    L.append(f" * 图像尺寸    : {width} x {height}  (宽 x 高)")
    L.append(" * 取模方式    : 阴码 / 逐列式 / 逆向")
    L.append(" *   阴码   : 数据位 1 = 点亮像素")
    L.append(" *   逐列式 : 每字节 = 一列的 8 个像素")
    L.append(" *   逆向   : bit0 = 页内最上一行, bit7 = 页内最下一行")
    L.append(" *")
    L.append(" * 数组结构    : 一维数组（所有帧顺序拼接）")
    L.append(" *   name[frame * STRIDE + page * WIDTH + col]")
    L.append(" *   frame -> 第几帧")
    L.append(" *   page  -> 第几页 (0 ~ PAGES-1)")
    L.append(" *   col   -> 第几列 (0 ~ WIDTH-1)")
    L.append(" *")
    L.append(" * 显示参数    :")
    L.append(f" *   X    (列) : 0 ~ {width - 1}   共 {width} 列")
    L.append(f" *   Page (页) : 0 ~ {pages - 1}   共 {pages} 页")
    L.append(" *")
    L.append(f" * 数据大小    : {count} x {stride} = {total} 字节 "
             f"({total / 1024.0:.1f} KB)")
    L.append(" *")
    L.append(" * ---------------- STM32 显示示例 ----------------")
    L.append(" *")
    L.append(" *   // 假设驱动已提供:")
    L.append(" *   //   void SSD1306_SetCursor(uint8_t col, uint8_t page);")
    L.append(" *   //   void SSD1306_WriteBytes(const uint8_t *buf, uint16_t len);")
    L.append(" *")
    L.append(" *   void OLED_ShowVideoFrame(uint16_t idx)")
    L.append(" *   {")
    L.append(f" *       if (idx >= {uname}_FRAME_COUNT) return;")
    L.append(" *")
    L.append(f" *       const uint8_t *p = &{name}[idx * {uname}_STRIDE];")
    L.append(" *")
    L.append(f" *       for (uint8_t page = 0; page < {uname}_PAGES; page++) {{")
    L.append(f" *           SSD1306_SetCursor(0, page);       // X = 0, Page = page")
    L.append(f" *           SSD1306_WriteBytes(&p[page * {uname}_WIDTH], "
             f"{uname}_WIDTH);")
    L.append(" *       }")
    L.append(" *   }")
    L.append(" *")
    L.append(" *   // 播放循环 (10 fps -> 每帧 100 ms)")
    L.append(" *   // uint16_t f = 0;")
    L.append(" *   // while (1) {")
    L.append(" *   //     OLED_ShowVideoFrame(f);")
    L.append(f" *   //     f = (f + 1) % {uname}_FRAME_COUNT;")
    L.append(" *   //     HAL_Delay(100);")
    L.append(" *   // }")
    L.append(" */")
    L.append("")
    L.append(f"#ifndef {guard}")
    L.append(f"#define {guard}")
    L.append("")
    L.append("#include <stdint.h>")
    L.append("")
    L.append(f"#define {uname}_FRAME_COUNT   ({count})")
    L.append(f"#define {uname}_WIDTH         ({width})")
    L.append(f"#define {uname}_HEIGHT        ({height})")
    L.append(f"#define {uname}_PAGES         ({pages})")
    L.append(f"#define {uname}_STRIDE        ({stride})   /* 每帧字节数 = PAGES * WIDTH */")
    L.append(f"#define {uname}_TOTAL_SIZE    ({total})   /* 总字节数 = FRAME_COUNT * STRIDE */")
    L.append("")
    L.append(f"/* {name}[frame * STRIDE + page * WIDTH + col] */")
    L.append(f"static const uint8_t {name}[{uname}_TOTAL_SIZE] = {{")

    # 按帧输出，每帧之间加注释分隔，便于定位
    for fi in range(count):
        chunk = flat_all[fi * stride:(fi + 1) * stride]
        L.append(f"    /* ---------- frame {fi} ---------- */")
        for i in range(0, chunk.size, 16):
            part = chunk[i:i + 16].tolist()
            L.append("    " + ", ".join(f"0x{b:02X}" for b in part) + ",")

    L.append("};")
    L.append("")
    L.append(f"#endif /* {guard} */")
    L.append("")

    with open(path, "w", encoding="utf-8") as fp:
        fp.write("\n".join(L))


# --------------------------------------------------------------------------- #
# 主流程
# --------------------------------------------------------------------------- #
def main():
    args = parse_args()

    if args.height % 8 != 0:
        sys.exit("错误: --height 必须是 8 的倍数 (SSD1306 为 64)")

    cap = cv2.VideoCapture(args.input)
    if not cap.isOpened():
        sys.exit(f"错误: 无法打开视频 {args.input}")

    src_fps = cap.get(cv2.CAP_PROP_FPS) or 25.0
    src_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    src_w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    src_h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    print("=" * 64)
    print(f"  源视频    : {args.input}")
    print(f"  分辨率    : {src_w} x {src_h} @ {src_fps:.2f} fps, 共 {src_count} 帧")
    print(f"  提取帧率  : {args.fps} fps")
    print("=" * 64)

    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8)) if args.clahe else None
    interval = 1.0 / args.fps
    next_time = 0.0
    frame_idx = 0
    frames = []

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        t = frame_idx / src_fps
        if t + 1e-9 >= next_time:
            next_time += interval

            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            if clahe is not None:
                gray = clahe.apply(gray)

            small = resize_frame(gray, args.height, args.max_width)
            if small is None:
                frame_idx += 1
                continue

            binary = binarize(small, args.threshold,
                              dither=not args.no_dither,
                              invert=args.invert)
            frames.append(binary)

            print(f"\r  已提取 {len(frames)} 帧 ...", end="", flush=True)

            if args.max_frames and len(frames) >= args.max_frames:
                break

        frame_idx += 1

    cap.release()
    print()

    if not frames:
        sys.exit("错误: 没有提取到任何帧")

    # 统一宽度（理论上每帧一致，保险起见取最大值并补零）
    width = max(f.shape[1] for f in frames)
    height = args.height
    pages = height // 8

    packed = []
    for f in frames:
        if f.shape[1] < width:
            pad = np.zeros((height, width), dtype=np.uint8)
            pad[:, :f.shape[1]] = f
            f = pad
        packed.append(pack_column_major(f, height))

    if args.preview:
        cv2.imwrite(args.preview, frames[0] * 255)
        print(f"  预览图已保存: {args.preview}")

    meta = {
        "src": args.input,
        "sw": src_w,
        "sh": src_h,
        "sfps": src_fps,
        "fps": args.fps,
    }
    write_header(args.output, args.name, packed, width, height, meta)

    stride = pages * width
    total = len(packed) * stride
    print("-" * 64)
    print(f"  输出文件    : {args.output}")
    print(f"  数组名      : {args.name}")
    print(f"  数组维度    : [{total}]  (一维，所有帧顺序拼接)")
    print(f"  帧数        : {len(packed)}")
    print(f"  每帧字节数  : {stride}")
    print(f"  图像尺寸    : {width} x {height}  (宽 x 高)")
    print(f"  取模方式    : 阴码 / 逐列式 / 逆向")
    print(f"  X    (列)   : 0 ~ {width - 1}   共 {width} 列")
    print(f"  Page (页)   : 0 ~ {pages - 1}   共 {pages} 页")
    print(f"  数据大小    : {total} 字节 ({total / 1024.0:.1f} KB)")
    print("-" * 64)
    if total > 32 * 1024:
        print("  提示: 数据较大，若 STM32 内部 Flash 放不下，可考虑")
        print("        - 降低 --fps 或使用 --max-frames 限制帧数")
        print("        - 存入外部 SPI Flash / SD 卡，运行时再读取")
    print("  完成 ✔")


if __name__ == "__main__":
    main()