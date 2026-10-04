"""
Builds the title screen parallax layers out of the night city background.

The pack has a sky, two rows of buildings and a fence (plus a bush the
artist's own preview does not use), one GBA background each. They are drawn
on a 380x180 canvas, bottom aligned, so the top 20 rows of sky are dropped
to fit the 160px screen.

A GBA background only wraps every 256 or 512px and the art repeats every 380
(328 for the back buildings), so each layer is extended to 512 by repeating
a slice of itself. The slice is picked so neither of its ends shows:
    sky              between columns that have no star on them
    buildings back   between gaps that are transparent top to bottom
    buildings front  between two building edges
    fence            a run of pickets, which repeat every 6px

Outputs, all 512x256 4bpp with the art in the top 160 rows:
    graphics/title_sky.bmp
    graphics/title_buildings_back.bmp
    graphics/title_buildings_front.bmp
    graphics/title_fence.bmp
    graphics/<name>.json      butano regular bg descriptors

Usage: python scripts/make_title_parallax.py
"""

import os
import struct

from PIL import Image

SOURCE_DIR = '/Users/rodrigocasale/Documents/proyects/gba_dev/juniors/city background/night'

BG_W = 512
BG_H = 256
SCREEN_H = 160

TRANSPARENT = (255, 0, 255)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')


def load_columns(file_name, first_column, period):
    """Returns the layer's height and one period of it as a list of columns."""
    image = Image.open(os.path.join(SOURCE_DIR, file_name)).convert('RGBA')
    pixels = image.load()
    columns = [tuple(pixels[x, y] for y in range(image.height))
               for x in range(first_column, first_column + period)]
    return image.height, columns


def extend(columns, start, description):
    """Returns BG_W columns: the whole period starting at start, then a slice to fill up."""
    period = len(columns)
    slice_width = BG_W - period
    print('    %s: columns %d..%d repeated' % (description, start, (start + slice_width) % period))
    return [columns[(start + x) % period] for x in range(period + slice_width)]


def find_start(columns, is_clean, description):
    """Finds a slice whose two ends land on columns where a cut does not show."""
    period = len(columns)
    slice_width = BG_W - period

    for start in range(period):
        end = (start + slice_width) % period

        if is_clean(columns, start) and is_clean(columns, end):
            return start

    raise SystemExit('no clean %dpx slice for %s' % (slice_width, description))


def no_star(columns, x):
    # A cut before column x is clean if x and the column before it are plain
    # sky, so no star is split in two. The sky is dithered, so what plain sky
    # looks like depends on whether the column is odd or even.
    return columns[x] == plain_sky(columns, x) and columns[x - 1] == plain_sky(columns, x - 1)


def plain_sky(columns, x):
    counts = {}

    for column in columns[x % 2::2]:
        counts[column] = counts.get(column, 0) + 1

    return max(counts, key=counts.get)


def is_gap(columns, x):
    return not any(pixel[3] for pixel in columns[x]) and not any(pixel[3] for pixel in columns[x - 1])


def top_of(column):
    return next((y for y, pixel in enumerate(column) if pixel[3]), len(column))


def is_building_edge(columns, x):
    return abs(top_of(columns[x]) - top_of(columns[x - 1])) > 4


def write_bg(name, height, columns):
    colors = sorted({pixel[:3] for column in columns for pixel in column if pixel[3]})

    if len(colors) > 15:
        raise SystemExit('%s has %d colors, a 4bpp background only has 15' % (name, len(colors)))

    if TRANSPARENT in colors:
        raise SystemExit('%s uses the transparent color' % name)

    palette = [TRANSPARENT] + colors
    palette += [(0, 0, 0)] * (16 - len(palette))

    # Bottom aligned on the screen. The sky is taller than it, so it loses its top rows.
    y_offset = SCREEN_H - height
    indexes = [[0] * BG_W for _ in range(BG_H)]

    for x, column in enumerate(columns):
        for y, pixel in enumerate(column):
            if pixel[3] and 0 <= y + y_offset < SCREEN_H:
                indexes[y + y_offset][x] = palette.index(pixel[:3])

    tiles = set()

    for tile_y in range(0, SCREEN_H, 8):
        for tile_x in range(0, BG_W, 8):
            rows = tuple(tuple(indexes[tile_y + y][tile_x:tile_x + 8]) for y in range(8))
            flips = (rows, rows[::-1], tuple(row[::-1] for row in rows),
                     tuple(row[::-1] for row in rows[::-1]))
            tiles.add(min(flips))

    row_size = BG_W // 2
    data = bytearray()

    for y in range(BG_H - 1, -1, -1):
        row = bytearray(row_size)

        for x in range(BG_W):
            row[x // 2] |= indexes[y][x] << (0 if x % 2 else 4)

        data += row

    offset = 14 + 40 + 16 * 4

    with open(os.path.join(GRAPHICS, name + '.bmp'), 'wb') as bmp:
        bmp.write(struct.pack('<2sIHHI', b'BM', offset + len(data), 0, 0, offset))
        bmp.write(struct.pack('<IiiHHIIiiII', 40, BG_W, BG_H, 1, 4, 0, len(data), 2834, 2834, 16, 16))

        for r, g, b in palette:
            bmp.write(struct.pack('<BBBB', b, g, r, 0))

        bmp.write(data)

    with open(os.path.join(GRAPHICS, name + '.json'), 'w') as json_file:
        json_file.write('{\n    "type": "regular_bg",\n    "bpp_mode": "bpp_4_manual"\n}\n')

    print('%s.bmp %d colors, %d tiles' % (name, len(colors), len(tiles)))
    return len(tiles)


def main():
    tiles = 0

    height, columns = load_columns('night sky.png', 0, 380)
    start = find_start(columns, no_star, 'sky')
    tiles += write_bg('title_sky', height, extend(columns, start, 'sky'))

    height, columns = load_columns('night buildings back.png', 0, 328)
    start = find_start(columns, is_gap, 'buildings back')
    tiles += write_bg('title_buildings_back', height, extend(columns, start, 'buildings back'))

    height, columns = load_columns('night buildings front.png', 0, 380)
    start = find_start(columns, is_building_edge, 'buildings front')
    tiles += write_bg('title_buildings_front', height, extend(columns, start, 'buildings front'))

    # The fence image is two periods with an end post on each side; the period
    # starting at column 16 has the lamp post once and pickets everywhere else.
    # The slice is 132px, 22 pickets, taken right from the start of that run.
    height, columns = load_columns('night fence.png', 16, 380)
    tiles += write_bg('title_fence', height, extend(columns, 0, 'fence'))

    # 4 backgrounds of 64x32 tiles take 16KB of the 64KB of background VRAM as
    # maps, which leaves room for 1536 4bpp tiles.
    print('%d tiles in total, 1536 fit' % tiles)


if __name__ == '__main__':
    main()
