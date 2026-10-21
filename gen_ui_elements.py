#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_ui_elements.py
================================================================
重新生成 ui_elements.hpp / ui_elements.cpp

  * 字形     : 内置 8x16 ASCII 点阵（每字形 8 列 x 16 行 = 2 Page）
  * 宽度     : 固定 —— 每字符严格占 8 列，不做左右裁剪
               （保证与旧头文件里的 WIDTH 宏一一对应）
  * 取模     : 阴码 / 逐列式 / 逆向
  * 数组布局 : buf[page * width + col]
               bit0 = 页内最上行，bit7 = 页内最下行
  * 接口     : 与旧文件完全一致（宏名 / 数组名 / 尺寸均不变）

用法:
    python gen_ui_elements.py                    # 输出到当前目录
    python gen_ui_elements.py -d ./image         # 输出到 ./image
    python gen_ui_elements.py -p preview.png     # 另外输出预览图
================================================================
"""

import argparse
import os

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
# 全局参数
# --------------------------------------------------------------------------- #
GLYPH_W = 8                       # 每字符固定 8 列
HEIGHT = 16                       # 高 16 像素
PAGES = HEIGHT // 8               # 2 Page


# --------------------------------------------------------------------------- #
# 要生成的条目（数组名 / 文本 / 宽度宏名）
# 顺序与旧文件保持一致
# --------------------------------------------------------------------------- #
TEXT_ITEMS = [
    ("text_welcome",       "WELCOME",       "TEXT_WELCOME_WIDTH"),
    ("text_led",       "LED",       "TEXT_LED_WIDTH"),
    ("text_image",     "IMAGE",     "TEXT_IMAGE_WIDTH"),
    ("text_anime",     "ANIME",     "TEXT_ANIME_WIDTH"),
    ("text_game",      "GAME",      "TEXT_GAME_WIDTH"),
    ("text_game_over", "GAME OVER", "TEXT_GAME_OVER_WIDTH"),
    ("text_source",    "SOURCE",    "TEXT_SOURCE_WIDTH"),
]

NUM_WIDTH_MACRO = "NUMBER_WIDTH"
NUM_ITEMS = [(f"number_{d}", d) for d in "0123456789"]

SYM_ITEMS = [
    ("symbol_slash", "/", "SYMBOL_SLASH_WIDTH"),
]


# --------------------------------------------------------------------------- #
# 渲染 & 取模
# --------------------------------------------------------------------------- #
def glyph(ch):
    """返回某字符的 16 字节位图（自上而下，bit7 在左）。"""
    hexs = FONT_8x16.get(ch) or FONT_8x16.get(ch.upper()) or _MISSING
    raw = bytes.fromhex(hexs)
    if len(raw) != 16:
        raw = (raw + bytes(16))[:16]
    return raw


def render(text):
    """
    固定宽度渲染：每个字形严格占 GLYPH_W 列，不做空白列裁剪。
    返回 shape=(HEIGHT, len(text) * GLYPH_W) 的 0/1 数组。
    """
    w = max(1, len(text) * GLYPH_W)
    canvas = np.zeros((HEIGHT, w), dtype=np.uint8)
    for i, ch in enumerate(text):
        g = glyph(ch)
        x0 = i * GLYPH_W
        for row in range(HEIGHT):
            b = g[row]
            for col in range(GLYPH_W):
                if b & (0x80 >> col):
                    canvas[row, x0 + col] = 1
    return canvas


def pack_column_major(binary):
    """阴码 / 逐列式 / 逆向。返回 shape=(PAGES, width)。"""
    h, w = binary.shape
    data = np.zeros((PAGES, w), dtype=np.uint8)
    for page in range(PAGES):
        for bit in range(8):
            y = page * 8 + bit
            if y >= h:
                break
            data[page] |= np.left_shift(binary[y].astype(np.uint8), bit)
    return data


def build(name, text):
    binimg = render(text)
    return {
        "name":   name,
        "text":   text,
        "width":  int(binimg.shape[1]),
        "pages":  pack_column_major(binimg),   # shape (PAGES, width)
        "binary": binimg,
    }


# --------------------------------------------------------------------------- #
# 生成 ui_elements.hpp
# --------------------------------------------------------------------------- #
def gen_hpp(path):
    L = []
    L.append("#pragma once")
    L.append("")
    L.append(f"#define TEXT_HEIGHT ({HEIGHT})")
    L.append(f"#define TEXT_PAGES ({PAGES})")
    L.append("")

    # ---- 文本宽度宏 ----
    for _, text, wm in TEXT_ITEMS:
        L.append(f"#define {wm} ({len(text) * GLYPH_W})")
    L.append("")

    # ---- 数字宽度宏 ----
    L.append(f"#define {NUM_WIDTH_MACRO} ({GLYPH_W})")
    L.append("")

    # ---- 符号宽度宏 ----
    for _, text, wm in SYM_ITEMS:
        L.append(f"#define {wm} ({len(text) * GLYPH_W})")
    L.append("")

    # ---- 文本 SIZE 宏 ----
    for name, _, wm in TEXT_ITEMS:
        L.append(f"#define {name.upper()}_SIZE ({wm} * TEXT_PAGES)")
    L.append("")

    # ---- 数字 SIZE 宏 ----
    for name, _ in NUM_ITEMS:
        L.append(f"#define {name.upper()}_SIZE ({NUM_WIDTH_MACRO} * TEXT_PAGES)")
    L.append("")

    # ---- 符号 SIZE 宏 ----
    for name, _, wm in SYM_ITEMS:
        L.append(f"#define {name.upper()}_SIZE ({wm} * TEXT_PAGES)")
    L.append("")

    # ---- extern 声明 ----
    for name, _, _ in TEXT_ITEMS:
        L.append(f"extern const unsigned char {name}[{name.upper()}_SIZE];")
    L.append("")
    for name, _ in NUM_ITEMS:
        L.append(f"extern const unsigned char {name}[{name.upper()}_SIZE];")
    L.append("")
    for name, _, _ in SYM_ITEMS:
        L.append(f"extern const unsigned char {name}[{name.upper()}_SIZE];")
    L.append("")

    with open(path, "w", encoding="utf-8", newline="\n") as fp:
        fp.write("\n".join(L))


# --------------------------------------------------------------------------- #
# 生成 ui_elements.cpp
# --------------------------------------------------------------------------- #
def gen_cpp(path, entries):
    L = [
        '#include "image/ui_elements.hpp"',
        "",
        "/*",
        " * 取模方式：阴码 / 逐列式 / 逆向",
        f" * 高度：TEXT_HEIGHT = {HEIGHT}，TEXT_PAGES = {PAGES}",
        " * 数组布局：buf[page * width + col]",
        " * bit0 = 页内最上行，bit7 = 页内最下行",
        " */",
        "",
    ]

    for e in entries:
        u = e["name"].upper()
        L.append(f"const unsigned char {e['name']}[{u}_SIZE] = {{")
        for page in range(PAGES):
            L.append(f"    /* page{page} */")
            for b in e["pages"][page]:
                L.append(f"    0x{int(b):02X},")
        L.append("};")
        L.append("")

    with open(path, "w", encoding="utf-8", newline="\n") as fp:
        fp.write("\n".join(L))


# --------------------------------------------------------------------------- #
# 预览图（可选）
# --------------------------------------------------------------------------- #
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

    try:
        from PIL import Image
        Image.fromarray(big).save(path)
    except ImportError:
        path = os.path.splitext(path)[0] + ".pgm"
        with open(path, "wb") as fp:
            fp.write(b"P5\n%d %d\n255\n" % (big.shape[1], big.shape[0]))
            fp.write(big.tobytes())


# --------------------------------------------------------------------------- #
# 主流程
# --------------------------------------------------------------------------- #
def main():
    ap = argparse.ArgumentParser(
        description="用内置 8x16 点阵字体重新生成 ui_elements.hpp / ui_elements.cpp",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    ap.add_argument("-d", "--dir", default=".", help="输出目录")
    ap.add_argument("-p", "--preview", default="", help="预览 PNG 路径（可选）")
    ap.add_argument("--scale", type=int, default=6, help="预览放大倍数")
    args = ap.parse_args()

    os.makedirs(args.dir, exist_ok=True)

    entries = []
    for name, text, _ in TEXT_ITEMS:
        entries.append(build(name, text))
    for name, text in NUM_ITEMS:
        entries.append(build(name, text))
    for name, text, _ in SYM_ITEMS:
        entries.append(build(name, text))

    hpp_path = os.path.join(args.dir, "ui_elements.hpp")
    cpp_path = os.path.join(args.dir, "ui_elements.cpp")

    gen_hpp(hpp_path)
    gen_cpp(cpp_path, entries)

    print("=" * 72)
    print("  字形     : 内置 8x16 ASCII 点阵（8 列 x 16 行 = 2 Page）")
    print("  宽度     : 固定 8 列 / 字形（不裁剪）")
    print("  取模     : 阴码 / 逐列式 / 逆向")
    print("=" * 72)
    print(f"  {'#':<3}{'数组名':<20}{'内容':<14}{'X(列)':>8}{'Page':>6}{'字节':>8}")
    print("-" * 72)
    total = 0
    for i, e in enumerate(entries):
        size = e["pages"].size
        total += size
        print(f"  {i:<3}{e['name']:<20}{e['text']:<14}"
              f"{e['width']:>8}{PAGES:>6}{size:>8}")
    print("-" * 72)
    print(f"  输出头文件 : {hpp_path}")
    print(f"  输出源文件 : {cpp_path}")
    print(f"  条目总数   : {len(entries)}")
    print(f"  总数据量   : {total} 字节 ({total / 1024.0:.1f} KB)")

    if args.preview:
        save_preview(args.preview, [e["binary"] for e in entries], args.scale)
        print(f"  预览图     : {args.preview}")

    print("-" * 72)
    print("  完成 ✔")


if __name__ == "__main__":
    main()