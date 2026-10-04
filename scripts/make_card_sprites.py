"""
Packs the individual card PNGs into Butano sprite sheets.

The source cards are 190x270 RGBA PNGs, which are 19x27 pixel art upscaled
10x, so they are shrunk back with nearest neighbour (lossless) and placed in
a 32x32 cell, the smallest GBA sprite size that holds 19x27.

Each suit sheet stacks its cells vertically in Poker::Rank order (TWO..ACE),
since card_sprite_utils.h uses the rank as the tiles index. All sheets share
one 16 color palette with the transparent color at index 0.

Outputs:
    graphics/cards_<suit>.bmp   13 cells, 32x416, 4bpp
    graphics/card_back.bmp      1 cell, 32x32, 4bpp
    graphics/<name>.json        butano sprite item descriptors

Usage: python scripts/make_card_sprites.py [light|dark]
"""

import os
import struct
import sys

from PIL import Image

CARDS_DIR = '/Users/rodrigocasale/Documents/proyects/gba_dev/juniors/cards'
SOURCE_SCALE = 10
CARD_W = 19
CARD_H = 27
CELL_W = 32
CELL_H = 32
CARD_X = (CELL_W - CARD_W) // 2
CARD_Y = (CELL_H - CARD_H) // 2

TRANSPARENT = (0, 255, 0)

# Poker::Rank order.
RANKS = ['2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K', 'A']

# Sheet name -> suit letter used by the source file names.
SUITS = {
    'cards_clubs': 'C',
    'cards_diamond': 'D',
    'cards_hearts': 'H',
    'cards_spades': 'P',
}

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')


def load_card(theme, name):
    path = os.path.join(CARDS_DIR, theme, name + '.png')
    image = Image.open(path).convert('RGBA')
    expected = (CARD_W * SOURCE_SCALE, CARD_H * SOURCE_SCALE)

    if image.size != expected:
        raise SystemExit('%s is %s, expected %s' % (path, image.size, expected))

    card = image.resize((CARD_W, CARD_H), Image.NEAREST)

    if card.resize(expected, Image.NEAREST).tobytes() != image.tobytes():
        raise SystemExit('%s is not a clean %dx upscale' % (path, SOURCE_SCALE))

    return card


def build_sheet(cards):
    sheet = Image.new('RGBA', (CELL_W, CELL_H * len(cards)), (0, 0, 0, 0))

    for index, card in enumerate(cards):
        sheet.paste(card, (CARD_X, index * CELL_H + CARD_Y))

    return sheet


def build_palette(sheets):
    colors = set()

    for sheet in sheets:
        for r, g, b, a in sheet.getdata():
            if a:
                colors.add((r, g, b))

    if TRANSPARENT in colors:
        raise SystemExit('transparent color %s is used by the cards' % (TRANSPARENT,))

    if len(colors) > 15:
        raise SystemExit('%d colors, a 4bpp sprite only has 15 plus transparent' % len(colors))

    palette = [TRANSPARENT] + sorted(colors)
    return palette + [(0, 0, 0)] * (16 - len(palette))


def write_bmp(path, sheet, palette):
    width, height = sheet.size
    row_size = ((width * 4 + 31) // 32) * 4
    pixels = sheet.load()
    data = bytearray()

    for y in range(height - 1, -1, -1):
        row = bytearray(row_size)

        for x in range(width):
            r, g, b, a = pixels[x, y]
            index = palette.index((r, g, b)) if a else 0
            row[x // 2] |= index << (0 if x % 2 else 4)

        data += row

    offset = 14 + 40 + 16 * 4

    with open(path, 'wb') as bmp:
        bmp.write(struct.pack('<2sIHHI', b'BM', offset + len(data), 0, 0, offset))
        bmp.write(struct.pack('<IiiHHIIiiII', 40, width, height, 1, 4, 0, len(data), 2834, 2834, 16, 16))

        for r, g, b in palette:
            bmp.write(struct.pack('<BBBB', b, g, r, 0))

        bmp.write(data)


def write_json(path):
    with open(path, 'w') as json_file:
        json_file.write('{\n    "type": "sprite",\n    "height": %d\n}\n' % CELL_H)


def main():
    theme = sys.argv[1] if len(sys.argv) > 1 else 'light'
    sheets = {'card_back': build_sheet([load_card(theme, 'BACK')])}

    for name, suit in SUITS.items():
        sheets[name] = build_sheet([load_card(theme, '%s-%s' % (rank, suit)) for rank in RANKS])

    palette = build_palette(sheets.values())

    for name, sheet in sheets.items():
        write_bmp(os.path.join(GRAPHICS, name + '.bmp'), sheet, palette)
        write_json(os.path.join(GRAPHICS, name + '.json'))
        print('%s.bmp %dx%d' % (name, sheet.width, sheet.height))


if __name__ == '__main__':
    main()
