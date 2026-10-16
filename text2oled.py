#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
text2oled.py  (内置 8x16 ASCII 点阵字体版)
================================================================
把字符串转成 SSD1306 OLED 用的 C 头文件，不依赖任何字体文件。

  * 字形 : 内置 8x16 ASCII 点阵（每字形 8 列 x 16 行 = 2 Page）
  * 宽度 : 自适应 —— 逐字形裁剪左右空白列后拼接
  * 取模 : 阴码 / 逐列式 / 逆向（与 video2oled.py 一致）
  * 每个字符串生成独立数组：name_xxx[page * WIDTH + col]

依赖: numpy  （只用它做数组操作，也可轻松去掉）
示例:
    python text2oled.py "Hello" "World" -o text.h -n text
    python text2oled.py -i lines.txt -o text.h -n text -p preview.png
================================================================
"""

import argparse
import os
import re
import sys

import numpy as np


# --------------------------------------------------------------------------- #
# 内置 8x16 点阵字体
# 每字形 16 字节，自上而下；每字节 bit7 = 最左列，bit0 = 最右列
# --------------------------------------------------------------------------- #
FONT_8x16 = {
    ' ': '00000000000000000000000000000000',
    '!': '00000018181818181818001818000000',
    '"': '00000066666624000000000000000000',
    '#': '0000006C6CFE6C6C6CFE6C6C00000000',
    '$': '000018183E60603C06067C18180000',
    '%': '000000C6CC183066C600000000000000',
    '&': '000000386C6C3876DCCC7600000000',
    "'": '00000018183000000000000000000000',
    '(': '0000000C1830303030180C00000000',
    ')': '00000030180C0C0C0C183000000000',
    '*': '00000000663CFF3C66000000000000',
    '+': '0000000018187E1818000000000000',
    ',': '000000000000000000001818300000',
    '-': '0000000000007E0000000000000000',
    '.': '000000000000000000001818000000',
    '/': '000000060C183060C0800000000000',
    '0': '0000003C66666E7666663C00000000',
    '1': '000000181838181818187E00000000',
    '2': '0000003C66060C1830667E00000000',
    '3': '0000003C6606061C0606663C000000',
    '4': '0000000C1C3C6CCCFF0C0C0C000000',
    '5': '0000007E60607C060606663C000000',
    '6': '0000001C3060607C6666663C000000',
    '7': '0000007E66060C1818181818000000',
    '8': '0000003C6666663C6666663C000000',
    '9': '0000003C6666663E06060C38000000',
    ':': '000000000018180000181800000000',
    ';': '000000000018180000181830000000',
    '<': '000000000C18306030180C00000000',
    '=': '0000000000007E007E000000000000',
    '>': '000000006030180C18306000000000',
    '?': '0000003C66060C1818001818000000',
    '@': '0000003C666E6E6062663C00000000',
    'A': '000000183C6666667E666666000000',
    'B': '0000007C6666667C6666667C000000',
    'C': '0000003C6660606060663C00000000',
    'D': '000000786C666666666C7800000000',
    'E': '0000007E6060607C60607E00000000',
    'F': '0000007E6060607C60606000000000',
    'G': '0000003C6660606E66663C00000000',
    'H': '000000666666667E66666600000000',
    'I': '0000003C1818181818183C00000000',
    'J': '0000001E0C0C0C0C0C6C3800000000',
    'K': '0000006666666C78786C6600000000',
    'L': '000000606060606060607E00000000',
    'M': '00000063777F6B6B63636300000000',
    'N': '00000066767E7E6E66666600000000',
    'O': '0000003C6666666666663C00000000',
    'P': '0000007C6666667C60606000000000',
    'Q': '0000003C66666666666C3600000000',
    'R': '0000007C6666667C6C666600000000',
    'S': '0000003C6660603C0606663C000000',
    'T': '0000007E1818181818181800000000',
    'U': '000000666666666666663C00000000',
    'V': '0000006666666666663C1800000000',
    'W': '0000006363636B6B7F776300000000',
    'X': '0000006666663C183C666600000000',
    'Y': '000000666666663C18181800000000',
    'Z': '0000007E06060C1830607E00000000',
    '[': '0000003C3030303030303C00000000',
    '\\': '000000C06030180C06020000000000',
    ']': '0000003C0C0C0C0C0C0C3C00000000',
    '^': '000000183C66000000000000000000',
    '_': '0000000000000000000000FF000000',
    '`': '00000030180C000000000000000000',
    'a': '0000000000003C063E66663E000000',
    'b': '0000006060607C6666667C00000000',
    'c': '0000000000003C6660663C00000000',
    'd': '0000000606063E6666663E00000000',
    'e': '0000000000003C667E603C00000000',
    'f': '0000001C30307C3030303000000000',
    'g': '0000000000003E66663E06663C0000',
    'h': '0000006060607C6666666600000000',
    'i': '000000181800381818183C00000000',
    'j': '0000000606000E06060666663C0000',
    'k': '000000606060666C78786C66000000',
    'l': '000000381818181818183C00000000',
    'm': '000000000000667F6B6B6363000000',
    'n': '0000000000007C6666666600000000',
    'o': '0000000000003C6666663C00000000',
    'p': '0000000000007C66667C6060600000',
    'q': '0000000000003E66663E0606060000',
    'r': '0000000000006E7660606000000000',
    's': '0000000000003C603C06663C000000',
    't': '0000003030307E3030301E00000000',
    'u': '000000000000666666663E00000000',
    'v': '0000000000006666663C18000000',
    'w': '00000000000063636B7F36000000',
    'x': '00000000000066663C183C66660000',
    'y': '0000000000006666663E06063C0000',
    'z': '0000000000007E060C18307E000000',
    '{': '0000000E181818701818180E000000',
    '|': '00000018181818181818181800000000',
    '}': '000000701818180E18181870000000',
    '~': '00000000000076DC00000000000000',
}

# 缺失字形用空心方块代替
_MISSING = '0000003C6666666666663C00000000'


# --------------------------------------------------------------------------- #
# 内置字体渲染
# --------------------------------------------------------------------------- #
def glyph(ch):
    """返回某字符的 16 字节位图（自上而下，bit7 在左）。"""
    hexs = FONT_8x16.get(ch) or FONT_8x16.get(ch.upper()) or _MISSING
    raw = bytes.fromhex(hexs)
    if len(raw) != 16:
        raw = (raw + bytes(16))[:16]
    return raw


def render_text(text, tracking=0):
    """
    渲染字符串为灰度图 numpy 数组 shape=(16, w)。
    每个字形占 8 列，字间可加 tracking 像素；最后裁掉左右空白列。
    """
    if not text:
        return np.zeros((16, 1), dtype=np.uint8)

    # 每个字形固定占 8 列 + tracking 间距
    total_w = len(text) * (8 + tracking) - tracking
    total_w = max(1, total_w)

    canvas = np.zeros((16, total_w), dtype=np.uint8)
    x = 0
    for ch in text:
        g = glyph(ch)
        for row in range(16):
            b = g[row]
            for col in range(8):
                if b & (0x80 >> col):
                    canvas[row, x + col] = 1
        x += 8 + tracking

    # 裁掉左右空白列 —— 宽度自适应
    cols = np.where(canvas.max(axis=0) > 0)[0]
    if cols.size == 0:
        return np.zeros((16, 1), dtype=np.uint8)
    return canvas[:, cols[0]: cols[-1] + 1]


# --------------------------------------------------------------------------- #
# 取模与输出
# --------------------------------------------------------------------------- #
def binarize(gray, invert=False):
    """已经只有 0/1，直接按需反色。"""
    return (1 - gray).astype(np.uint8) if invert else gray.astype(np.uint8)


def pack_column_major(binary, height):
    """阴码 / 逐列式 / 逆向。返回 shape=(pages, width)。"""
    h, w = binary.shape
    pages = height // 8
    data = np.zeros((pages, w), dtype=np.uint8)
    for page in range(pages):
        for bit in range(8):
            y = page * 8 + bit
            if y >= h:
                break
            data[page] |= np.left_shift(binary[y].astype(np.uint8), bit)
    return data


def make_c_name(base, text, used):
    tag = re.sub(r"[^0-9A-Za-z_]", "_", text)
    tag = re.sub(r"_+", "_", tag).strip("_")[:24] or "blank"
    if tag[0].isdigit():
        tag = "n" + tag
    name = f"{base}_{tag}"
    i = 2
    while name in used:
        name = f"{base}_{tag}_{i}"
        i += 1
    used.add(name)
    return name


def write_header(path, entries, meta):
    base = meta["name"]
    uname = base.upper()
    guard = f"__{uname}_H__"

    L = [
        "/**",
        f" * @file    {os.path.basename(path)}",
        " * @brief   由 text2oled.py 自动生成，请勿手动修改",
        " *",
        f" * 字形        : 内置 8x16 ASCII 点阵（每字形高 {meta['height']}px = {meta['pages']} Page）",
        " * 宽度        : 自适应（逐字形裁剪空白列后拼接）",
        " * 取模方式    : 阴码 / 逐列式 / 逆向",
        " *   阴码   : 数据位 1 = 点亮像素",
        " *   逐列式 : 每字节 = 一列的 8 个像素",
        " *   逆向   : bit0 = 页内最上一行, bit7 = 页内最下一行",
        " *",
        " * 数组结构    : name_xxx[page * WIDTH + col]",
        " *",
        " * 字符串一览  :",
    ]
    for e in entries:
        L.append(f" *   {e['cname']:<24} X(列) = {e['width']:<4} "
                 f"Page = {e['pages']}   {e['flat'].size:>5} 字节   \"{e['text']}\"")
    L += [
        " *",
        " * ---------------- STM32 显示示例 ----------------",
        " *",
        " *   void OLED_ShowText(const uint8_t *d, uint8_t pages, uint8_t w)",
        " *   {",
        " *       for (uint8_t p = 0; p < pages; p++) {",
        " *           SSD1306_SetCursor(0, p);",
        " *           SSD1306_WriteBytes(&d[p * w], w);",
        " *       }",
        " *   }",
        " *",
        " */",
        "",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <stdint.h>",
        "",
        f"#define {uname}_COUNT   ({len(entries)})",
        "",
    ]

    for e in entries:
        u = e["cname"].upper()
        L.append("")
        L.append("/* " + "-" * 62 + " */")
        L.append(f"/* \"{e['text']}\"   X(列) = {e['width']}   Page = {e['pages']}   "
                 f"共 {e['flat'].size} 字节 */")
        L.append(f"#define {u}_X       ({e['width']})")
        L.append(f"#define {u}_PAGES   ({e['pages']})")
        L.append(f"#define {u}_SIZE    ({u}_PAGES * {u}_X)")
        L.append("")
        L.append(f"/* {e['cname']}[page * {u}_X + col] */")
        L.append(f"static const uint8_t {e['cname']}[{u}_SIZE] = {{")
        data = e["flat"]
        for i in range(0, data.size, 16):
            part = data[i:i + 16].tolist()
            L.append("    " + ", ".join(f"0x{b:02X}" for b in part) + ",")
        L.append("};")

    L += ["", f"#endif /* {guard} */", ""]
    with open(path, "w", encoding="utf-8") as fp:
        fp.write("\n".join(L))


def save_preview(path, rows, scale=6):
    gap = 2
    total_h = sum(r.shape[0] for r in rows) + gap * (len(rows) - 1)
    total_w = max(r.shape[1] for r in rows)
    canvas = np.zeros((total_h, total_w), dtype=np.uint8)
    y = 0
    for r in rows:
        canvas[y:y + r.shape[0], :r.shape[1]] = r * 255
        y += r.shape[0] + gap
    big = np.kron(canvas, np.ones((scale, scale), dtype=np.uint8))

    # 用最小 PNG 写出（避免引入 Pillow）
    try:
        from PIL import Image
        Image.fromarray(big).save(path)
    except ImportError:
        # 退化：手写 PGM
        path = os.path.splitext(path)[0] + ".pgm"
        with open(path, "wb") as fp:
            fp.write(b"P5\n%d %d\n255\n" % (big.shape[1], big.shape[0]))
            fp.write(big.tobytes())


# --------------------------------------------------------------------------- #
# 主流程
# --------------------------------------------------------------------------- #
def parse_args():
    p = argparse.ArgumentParser(
        description="字符串 -> 内置 8x16 点阵 -> SSD1306 列式阴码头文件",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    p.add_argument("texts", nargs="*", help="要转换的字符串（可多个）")
    p.add_argument("-i", "--from-file", default="", help="从文件读取，每行一个字符串")
    p.add_argument("-o", "--output", default="text_arrays.h", help="输出头文件")
    p.add_argument("-n", "--name", default="text", help="C 数组名前缀")
    p.add_argument("--height", type=int, default=16, help="字符高度（必须是 8 的倍数）")
    p.add_argument("--tracking", type=int, default=0, help="字形间额外像素间距")
    p.add_argument("--invert", action="store_true", help="反色（阴码 <-> 阳码）")
    p.add_argument("-p", "--preview", default="", help="保存预览 PNG（可选）")
    p.add_argument("--scale", type=int, default=6, help="预览放大倍数")
    return p.parse_args()


def main():
    args = parse_args()

    if args.height != 16:
        sys.exit("错误: 内置点阵字体高度固定为 16，无法改成其它高度")

    texts = list(args.texts)
    if args.from_file:
        with open(args.from_file, "r", encoding="utf-8") as fp:
            for line in fp:
                line = line.rstrip("\r\n")
                if line:
                    texts.append(line)
    if not texts:
        sys.exit("错误: 没有输入任何字符串")

    height = 16
    pages = height // 8

    print("=" * 72)
    print(f"  字形      : 内置 8x16 ASCII 点阵")
    print(f"  高度      : {height} px  ({pages} 个 Page)")
    print(f"  宽度      : 自适应")
    print(f"  取模      : 阴码 / 逐列式 / 逆向")
    print("=" * 72)

    entries = []
    used = set()
    for t in texts:
        gray = render_text(t, args.tracking)
        binary = binarize(gray, invert=args.invert)
        packed = pack_column_major(binary, height)
        entries.append({
            "text": t,
            "cname": make_c_name(args.name, t, used),
            "width": int(binary.shape[1]),
            "pages": pages,
            "flat": packed.reshape(-1),
            "binary": binary,
        })

    print()
    print("-" * 72)
    print(f"  {'#':<3}{'字符串':<26}{'数组名':<22}{'X(列)':>7}{'Page':>6}{'字节':>8}")
    print("-" * 72)
    for i, e in enumerate(entries):
        t = e["text"]
        if len(t) > 22:
            t = t[:21] + "…"
        print(f"  {i:<3}{t:<26}{e['cname']:<22}"
              f"{e['width']:>7}{e['pages']:>6}{e['flat'].size:>8}")
    print("-" * 72)

    if args.preview:
        save_preview(args.preview, [e["binary"] for e in entries], args.scale)
        print(f"  预览图已保存: {args.preview}")

    write_header(args.output, entries, {"name": args.name, "height": height, "pages": pages})

    total = sum(e["flat"].size for e in entries)
    print(f"  输出文件  : {args.output}")
    print(f"  字符串数  : {len(entries)}")
    print(f"  总数据量  : {total} 字节 ({total/1024.0:.1f} KB)")
    print("-" * 72)
    print("  完成 ✔")


if __name__ == "__main__":
    main()