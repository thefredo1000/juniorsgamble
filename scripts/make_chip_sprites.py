"""
Builds the chip sprites in the style of the "fiches addon" sheet.

The sheet's chips are 32px wide, too big next to 19px cards, and pixel art
this detailed does not survive a 3/4 shrink: the inner ring breaks up and the
stripes come out uneven. So the chip is redrawn here at 24px with the same
parts (outline, lit and shaded rim, ring, four stripes, corner dashes) and
only the colors are taken from the sheet.

Each pixel is drawn with a role letter rather than a color. A role's color
in each of the 8 chip colors is read from one known pixel of that chip in the
sheet, which gives one 16 color palette per chip color: the shapes are stored
once and recolored.

Outputs:
    graphics/chips.bmp        single chip, 24x22 in a 32x32 sprite, 4bpp
    graphics/chip_stack.bmp   stack, 24x33 in a 32x64 sprite, 4bpp
    graphics/chip_slot.bmp    empty bet slot, the single chip's silhouette
    graphics/<name>.json      butano sprite item descriptors
    include/chip_palettes.h   one bn::sprite_palette_item per chip color

Usage: python scripts/make_chip_sprites.py
"""

import os
import struct

from PIL import Image

SHEET = '/Users/rodrigocasale/Documents/proyects/gba_dev/juniors/Poker cards 1.3/fiches addon (Poker Cards).png'

# The sheet is a grid of 48x48 cells, 32px of chip and 16px of gap. Each color
# takes four columns (single chip plus three stacks), four colors per half.
CELL = 48
SHEET_CHIP_W = 32
STACKS_PER_COLOR = 4

# Palette order. The first six are the bet values 1, 2, 4, 8, 16 and 32.
# name -> (color column block, row) in the sheet.
COLORS = [
    ('white', (0, 1)),
    ('red', (0, 0)),
    ('blue', (1, 0)),
    ('green', (0, 2)),
    ('black', (0, 3)),
    ('purple', (1, 1)),
    ('pink', (1, 2)),
    ('yellow', (1, 3)),
]

# Role letter -> pixel of the sheet's single chip that has that role.
ROLES = {
    'a': (11, 0),   # outline
    'h': (15, 10),  # face
    'j': (11, 1),   # lit rim
    'f': (1, 19),   # shaded rim, stack ridges
    'e': (14, 1),   # stripes, ring corners
    'd': (14, 28),  # shaded stripe
    'b': (7, 9),    # ring
    'i': (12, 6),   # ring bevel
    'c': (8, 4),    # corner dash
    'r': (1, 19),   # stack ridge, same as the shaded rim
    's': (15, 10),  # between stack ridges, same as the face
}

# Colors that are not read from the sheet. The white chip's shade is so close
# to its face that the stack's side looks as bright as its top and the stack
# reads as a flat pill, so its ridges get a darker pair of greys.
ROLE_OVERRIDES = {
    'white': {
        'r': (129, 144, 148),
        's': (168, 181, 178),
    },
}

CHIP_W = 24
SPRITE_W = 32

# First and last column of every row of the top half of the chip, the eight
# full width rows in the middle come after it and then it is mirrored.
CHIP_EDGE_SPANS = [(8, 15), (6, 17), (4, 19), (3, 20), (2, 21), (1, 22), (1, 22)]
CHIP_SPANS = CHIP_EDGE_SPANS + [(0, CHIP_W - 1)] * 8 + CHIP_EDGE_SPANS[::-1]
CHIP_H = len(CHIP_SPANS)
STRIPE_COLUMNS = (10, 11, 12, 13)

# How much taller than a single chip the stack is.
STACK_EXTRA_H = 11

# Sprite name -> sprite height.
SPRITE_HEIGHTS = {
    'chips': 32,
    'chip_stack': 64,
}

TRANSPARENT = (0, 255, 0)

# The bet slot is the chip's silhouette pressed into the table: a rim lighter
# than the table's purples and a fill darker than them.
SLOT_RIM = (132, 78, 112)
SLOT_FILL = (52, 30, 46)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')
INCLUDE = os.path.join(ROOT, 'include')


def has_outside_neighbor(inside, x, y):
    return not (inside(x - 1, y) and inside(x + 1, y) and inside(x, y - 1) and inside(x, y + 1))


def draw_chip():
    """Returns the single chip as rows of role letters, None where transparent."""
    def inside(x, y):
        return 0 <= y < CHIP_H and CHIP_SPANS[y][0] <= x <= CHIP_SPANS[y][1]

    chip = [[None] * CHIP_W for _ in range(CHIP_H)]

    for y in range(CHIP_H):
        for x in range(CHIP_W):
            if inside(x, y):
                chip[y][x] = 'a' if has_outside_neighbor(inside, x, y) else 'h'

    # Rim: lit along the top of the outline, shaded along the bottom.
    for y in range(CHIP_H):
        for x in range(CHIP_W):
            if chip[y][x] == 'h':
                neighbors = ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1))

                if any(inside(nx, ny) and chip[ny][nx] == 'a' for nx, ny in neighbors):
                    if y <= 6:
                        chip[y][x] = 'j'
                    elif y >= 14:
                        chip[y][x] = 'f'

    def put(x, y, role):
        chip[y][x] = role
        chip[y][CHIP_W - 1 - x] = role

    # Ring: an octagon, white where the stripes and the corners cross it.
    for x in (8, 9):
        put(x, 5, 'b')
        put(x, 16, 'b')

    for x in (10, 11):
        put(x, 5, 'e')
        put(x, 16, 'e')

    for x in range(8, 12):
        put(x, 6, 'i')

    for x, y in ((7, 6), (6, 7), (6, 14), (7, 15)):
        put(x, y, 'e')

    for y in (8, 13):
        put(5, y, 'b')

    for y in range(9, 13):
        put(5, y, 'e')

    # Corner dashes.
    put(4, 4, 'c')
    put(5, 5, 'e')
    put(4, 17, 'c')
    put(5, 16, 'e')

    # Stripes.
    for x in (10, 11):
        for y in (1, 2, 3, 18, 19):
            put(x, y, 'e')

        put(x, 20, 'd')

    for y in range(9, 13):
        for x in (1, 2, 3):
            put(x, y, 'e')

    return chip


def draw_stack(chip):
    """Returns the stack: the chip on top and ridges that follow its lower rim below."""
    height = CHIP_H + STACK_EXTRA_H

    def inside(x, y):
        return 0 <= x < CHIP_W and any(0 <= y - drop < CHIP_H and chip[y - drop][x]
                                       for drop in range(STACK_EXTRA_H + 1))

    stack = [[None] * CHIP_W for _ in range(height)]

    for x in range(CHIP_W):
        face_rows = [y for y in range(CHIP_H) if chip[y][x] not in (None, 'a')]
        face_bottom = max(face_rows) if face_rows else None

        for y in range(height):
            if not inside(x, y):
                continue

            if has_outside_neighbor(inside, x, y):
                stack[y][x] = 'a'
            elif face_bottom is not None and y <= face_bottom:
                stack[y][x] = chip[y][x]
            else:
                ridge = (y - face_bottom) % 2 == 0

                if x in STRIPE_COLUMNS:
                    stack[y][x] = 'd' if ridge else 'e'
                else:
                    stack[y][x] = 'r' if ridge else 's'

    return stack


def load_palettes(sheet):
    """Returns the role letters in palette order and one 16 color palette per chip color."""
    letters = sorted(ROLES)
    palettes = []

    for name, (block, row) in COLORS:
        x = block * STACKS_PER_COLOR * CELL
        crop = sheet.crop((x, row * CELL, x + SHEET_CHIP_W, (row + 1) * CELL))
        crop = crop.crop(crop.getbbox())
        overrides = ROLE_OVERRIDES.get(name, {})
        palette = [TRANSPARENT] + [overrides.get(letter) or crop.getpixel(ROLES[letter])[:3]
                                   for letter in letters]
        palettes.append(palette + [(0, 0, 0)] * (16 - len(palette)))

    return letters, palettes


def sprite_indexes(rows, sprite_height, index_of):
    """Centers the drawing in a SPRITE_W x sprite_height sprite."""
    x_offset = (SPRITE_W - CHIP_W) // 2
    y_offset = (sprite_height - len(rows)) // 2
    indexes = [0] * (SPRITE_W * sprite_height)

    for y, row in enumerate(rows):
        for x, role in enumerate(row):
            if role:
                indexes[(y + y_offset) * SPRITE_W + x + x_offset] = index_of(x, y, role)

    return indexes


def write_bmp(path, width, height, indexes, palette):
    row_size = ((width * 4 + 31) // 32) * 4
    data = bytearray()

    for y in range(height - 1, -1, -1):
        row = bytearray(row_size)

        for x in range(width):
            row[x // 2] |= indexes[y * width + x] << (0 if x % 2 else 4)

        data += row

    offset = 14 + 40 + 16 * 4

    with open(path, 'wb') as bmp:
        bmp.write(struct.pack('<2sIHHI', b'BM', offset + len(data), 0, 0, offset))
        bmp.write(struct.pack('<IiiHHIIiiII', 40, width, height, 1, 4, 0, len(data), 2834, 2834, 16, 16))

        for r, g, b in palette:
            bmp.write(struct.pack('<BBBB', b, g, r, 0))

        bmp.write(data)


def write_slot(chip):
    sprite_height = SPRITE_HEIGHTS['chips']

    def inside(x, y):
        return 0 <= y < CHIP_H and 0 <= x < CHIP_W and chip[y][x] is not None

    indexes = sprite_indexes(chip, sprite_height,
                             lambda x, y, role: 1 if has_outside_neighbor(inside, x, y) else 2)
    palette = [TRANSPARENT, SLOT_RIM, SLOT_FILL] + [(0, 0, 0)] * 13
    write_bmp(os.path.join(GRAPHICS, 'chip_slot.bmp'), SPRITE_W, sprite_height, indexes, palette)
    write_json(os.path.join(GRAPHICS, 'chip_slot.json'), sprite_height)
    print('chip_slot.bmp %dx%d' % (SPRITE_W, sprite_height))


def write_json(path, height):
    with open(path, 'w') as json_file:
        json_file.write('{\n    "type": "sprite",\n    "height": %d\n}\n' % height)


def write_header(path, palettes):
    lines = [
        '// Generated by scripts/make_chip_sprites.py, do not edit.',
        '#ifndef CHIP_PALETTES_H',
        '#define CHIP_PALETTES_H',
        '',
        '#include "bn_color.h"',
        '#include "bn_bpp_mode.h"',
        '#include "bn_sprite_palette_item.h"',
        '',
        'namespace Game',
        '{',
        '    // The chips and chip_stack sprites are drawn once and recolored with these.',
        '    // Indexes 0..5 are the bet values 1, 2, 4, 8, 16 and 32.',
        '    //     ' + ', '.join(name for name, _ in COLORS),
        '    constexpr int chip_palette_count = %d;' % len(palettes),
        '',
        '    inline constexpr bn::color chip_palette_colors[chip_palette_count][16] = {',
    ]

    for palette in palettes:
        colors = ', '.join('bn::color(%d, %d, %d)' % (r >> 3, g >> 3, b >> 3) for r, g, b in palette)
        lines.append('        { %s },' % colors)

    lines += [
        '    };',
        '',
        '    [[nodiscard]] constexpr bn::sprite_palette_item chip_palette_item(int index)',
        '    {',
        '        return bn::sprite_palette_item(chip_palette_colors[index], bn::bpp_mode::BPP_4);',
        '    }',
        '}',
        '',
        '#endif // CHIP_PALETTES_H',
        '',
    ]

    with open(path, 'w') as header:
        header.write('\n'.join(lines))


def main():
    sheet = Image.open(SHEET).convert('RGBA')
    letters, palettes = load_palettes(sheet)
    chip = draw_chip()
    drawings = {'chips': chip, 'chip_stack': draw_stack(chip)}

    for name, rows in drawings.items():
        sprite_height = SPRITE_HEIGHTS[name]
        indexes = sprite_indexes(rows, sprite_height, lambda x, y, role: letters.index(role) + 1)
        write_bmp(os.path.join(GRAPHICS, name + '.bmp'), SPRITE_W, sprite_height, indexes, palettes[0])
        write_json(os.path.join(GRAPHICS, name + '.json'), sprite_height)
        print('%s.bmp %dx%d (chip %dx%d)' % (name, SPRITE_W, sprite_height, CHIP_W, len(rows)))

    write_slot(chip)
    write_header(os.path.join(INCLUDE, 'chip_palettes.h'), palettes)
    print('chip_palettes.h %d palettes, %d indexes' % (len(palettes), len(letters) + 1))


if __name__ == '__main__':
    main()
