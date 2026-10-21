#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
image2oled.py
================================================================
把一张或多张图片转换成 SSD1306 OLED 用的 C 头文件。

  - 高度固定为 64（可改，必须是 8 的倍数）
  - 宽度按原图宽高比自适应，上限 128（可改）；超宽时居中裁剪
  - 阴码 / 逐列式 / 逆向 取模，与 video2oled.py 完全兼容
  - 支持单图 / 多图 / 目录 / 通配符，多图会统一宽度并居中补黑，
    按帧顺序拼接成一维数组：
        name[frame * STRIDE + page * WIDTH + col]
  - 头文件里自动标注每张图的源文件、原始分辨率与输出尺寸

依赖:
    pip install opencv-python numpy

示例:
    python image2oled.py logo.png
    python image2oled.py logo.png -o logo.h -n logo
    python image2oled.py frames/ -o anim.h -n anim
    python image2oled.py a.png b.png c.png -o seq.h -n seq --invert -p preview.png
================================================================
"""

import argparse
import glob
import os
import sys

import numpy as np

try:
    import cv2
except ImportError:
    sys.exit("错误: 未安装 OpenCV，请执行  pip install opencv-python")


IMG_EXTS = (".png", ".jpg", ".jpeg", ".bmp", ".gif", ".webp", ".tif", ".tiff")


# --------------------------------------------------------------------------- #
# 命令行参数
# --------------------------------------------------------------------------- #
def parse_args():
    p = argparse.ArgumentParser(
        description="图片 -> 高度 64 / 宽度自适应 -> SSD1306 列式阴码头文件",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    p.add_argument("input", nargs="+",
                   help="图片文件 / 目录 / 通配符（可一次给多个）")
    p.add_argument("-o", "--output", default="image_frames.h", help="输出头文件路径")
    p.add_argument("-n", "--name", default="image_frames", help="生成的 C 数组名")
    p.add_argument("--height", type=int, default=64, help="目标高度(必须是 8 的倍数)")
    p.add_argument("--max-width", type=int, default=128, help="最大宽度(SSD1306 通常为 128)")
    p.add_argument("--threshold", type=int, default=128, help="二值化阈值 0~255")
    p.add_argument("--no-dither", action="store_true", help="关闭 Floyd-Steinberg 抖动, 用硬阈值")
    p.add_argument("--invert", action="store_true", help="反色(相当于阴码 <-> 阳码)")
    p.add_argument("--clahe", action="store_true", help="启用自适应直方图均衡, 提升对比度")
    p.add_argument("-p", "--preview", default="", help="把第一张结果保存为 PNG 预览(调试用)")
    return p.parse_args()


# --------------------------------------------------------------------------- #
# 输入收集
# --------------------------------------------------------------------------- #
def collect_inputs(items):
    """把文件、目录、通配符统一展开成一个图片路径列表（保序去重）。"""
    files = []
    for it in items:
        if os.path.isdir(it):
            for nm in sorted(os.listdir(it)):
                if nm.lower().endswith(IMG_EXTS):
                    files.append(os.path.join(it, nm))
        elif any(ch in it for ch in "*?["):
            files.extend(sorted(glob.glob(it)))
        else:
            files.append(it)

    seen, out = set(), []
    for f in files:
        if f not in seen:
            seen.add(f)
            out.append(f)
    return out


# --------------------------------------------------------------------------- #
# 图像处理
# --------------------------------------------------------------------------- #
def load_gray(path):
    """读图并转灰度；带 Alpha 的 PNG 先合成到黑底上。读失败返回 None。"""
    img = cv2.imread(path, cv2.IMREAD_UNCHANGED)
    if img is None:
        return None

    if img.ndim == 3 and img.shape[2] == 4:          # BGRA -> 黑底合成
        alpha = img[:, :, 3].astype(np.float32) / 255.0
        bgr = img[:, :, :3].astype(np.float32)
        img = (bgr * alpha[..., None]).astype(np.uint8)

    if img.ndim == 3:
        return cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return img


def resize_to_height(gray, target_h, max_w):
    """
    等比缩放到高度 = target_h，宽度自适应；
    若缩放后宽度超过 max_w，则从中间水平裁剪到 max_w（高度不变）。
    """
    h, w = gray.shape[:2]
    if h == 0 or w == 0:
        return None

    new_w = max(1, int(round(w * target_h / h)))
    interp = cv2.INTER_AREA if new_w < w else cv2.INTER_LINEAR
    resized = cv2.resize(gray, (new_w, target_h), interpolation=interp)

    if new_w > max_w:
        x0 = (new_w - max_w) // 2
        resized = resized[:, x0:x0 + max_w]

    return resized


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
def write_header(path, name, frames, width, height, meta):
    """
    frames : list of (filename, np.ndarray shape=(pages, width))
    输出一维数组: name[frame * STRIDE + page * WIDTH + col]
    """
    pages = height // 8
    count = len(frames)
    uname = name.upper()
    guard = f"__{uname}_H__"
    stride = pages * width                       # 一帧的字节数
    total = count * stride

    flat_all = np.concatenate([d.reshape(-1) for _, d in frames]) \
        if frames else np.zeros(0, dtype=np.uint8)

    srcs = meta["sources"]

    L = []
    L.append("/**")
    L.append(f" * @file    {os.path.basename(path)}")
    L.append(" * @brief   由 image2oled.py 自动生成，请勿手动修改")
    L.append(" *")
    if len(srcs) == 1:
        fname, sw, sh, ow = srcs[0]
        L.append(f" * 源图片      : {fname}  ({sw} x {sh})")
        L.append(f" * 输出尺寸    : {ow} x {height}  (宽 x 高，宽度按比例自适应)")
    else:
        L.append(f" * 源图片      : 共 {len(srcs)} 张")
        for i, (fname, sw, sh, ow) in enumerate(srcs):
            L.append(f" *   [{i:2d}] {fname:<28s} {sw} x {sh}  ->  {ow} x {height}")
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
    L.append(" *   void OLED_ShowImage(uint16_t idx)")
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

    # 按帧输出，每帧前加注释，标注源文件
    for fi in range(count):
        fname = frames[fi][0]
        chunk = flat_all[fi * stride:(fi + 1) * stride]
        L.append(f"    /* ---------- frame {fi}: {fname} ---------- */")
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

    files = collect_inputs(args.input)
    if not files:
        sys.exit("错误: 没有找到任何图片")

    print("=" * 64)
    print(f"  输入        : {len(files)} 张图片")
    print(f"  目标高度    : {args.height} px   最大宽度: {args.max_width} px")
    print("=" * 64)

    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8)) if args.clahe else None

    processed = []      # (filename, binary)
    sources = []        # (filename, src_w, src_h, out_w)

    for f in files:
        gray = load_gray(f)
        if gray is None:
            print(f"  跳过(无法读取): {f}")
            continue

        sh, sw = gray.shape[:2]
        if clahe is not None:
            gray = clahe.apply(gray)

        small = resize_to_height(gray, args.height, args.max_width)
        if small is None:
            print(f"  跳过(图像无效): {f}")
            continue

        binary = binarize(small, args.threshold,
                          dither=not args.no_dither,
                          invert=args.invert)

        fname = os.path.basename(f)
        processed.append((fname, binary))
        sources.append((fname, sw, sh, binary.shape[1]))
        print(f"  {fname:<30s} {sw} x {sh}  ->  {binary.shape[1]} x {args.height}")

    if not processed:
        sys.exit("错误: 没有处理出任何可用数据")

    # 统一宽度（多图尺寸不一致时，取最大宽度，其余居中补黑）
    width = max(b.shape[1] for _, b in processed)
    height = args.height

    padded = []
    for fname, b in processed:
        if b.shape[1] < width:
            canvas = np.zeros((height, width), dtype=np.uint8)
            x0 = (width - b.shape[1]) // 2
            canvas[:, x0:x0 + b.shape[1]] = b
            b = canvas
        padded.append((fname, b))

    packed = [(fname, pack_column_major(b, height)) for fname, b in padded]

    if args.preview:
        cv2.imwrite(args.preview, padded[0][1] * 255)
        print(f"  预览图已保存: {args.preview}")

    meta = {"sources": sources}
    write_header(args.output, args.name, packed, width, height, meta)

    pages = height // 8
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
        print("        - 缩小 --max-width，或用 --no-dither 减小编码体积")
        print("        - 存入外部 SPI Flash / SD 卡，运行时再读取")
    print("  完成 ✔")


if __name__ == "__main__":
    main()