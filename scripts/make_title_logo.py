"""
Turns the title art into the sprites the title screen fades in.

The four backgrounds are taken by the parallax city, so the logo has to be
sprites. It is cut into a 3x2 grid of 64x64 sprites, one frame each.

The source is a 240x160 8bpp BMP with the logo on a green background, which
needs two fixes before it can be a sprite:
    - Its transparent green is palette index 1 and butano wants index 0.
    - Its outline was antialiased against the green, which leaves a fringe of
      dark greens that would show against the city. Those are blends of black
      and the background, so the lighter half becomes transparent and the
      darker half becomes black.

Outputs:
    graphics/title_logo.bmp    64x384 8bpp, 6 frames, left to right, top to bottom
    graphics/title_logo.json   butano sprite item descriptor

Usage: python scripts/make_title_logo.py
"""

import os
import struct

from PIL import Image

SOURCE = '/Users/rodrigocasale/Documents/proyects/gba_dev/juniors/title.bmp'

SPRITE_SIZE = 64
GRID_COLUMNS = 3
GRID_ROWS = 2

TRANSPARENT = (0, 254, 0)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')


def clean(color):
    """Returns the color to use for a source pixel, None if transparent."""
    r, g, b = color

    # Pure greens only come from the background and its blend with the outline.
    if r == 0 and b == 0 and g:
        return None if g >= 128 else (0, 0, 0)

    return color


def main():
    source = Image.open(SOURCE).convert('RGB')
    pixels = [clean(color) for color in source.getdata()]
    width, height = source.size

    xs = [index % width for index, color in enumerate(pixels) if color]
    ys = [index // width for index, color in enumerate(pixels) if color]
    left, top, right, bottom = min(xs), min(ys), max(xs) + 1, max(ys) + 1
    logo_width = right - left
    logo_height = bottom - top
    grid_width = SPRITE_SIZE * GRID_COLUMNS
    grid_height = SPRITE_SIZE * GRID_ROWS

    if logo_width > grid_width or logo_height > grid_height:
        raise SystemExit('the %dx%d logo does not fit a %dx%d grid'
                         % (logo_width, logo_height, grid_width, grid_height))

    colors = sorted({color for color in pixels if color})
    palette = [TRANSPARENT] + colors
    colors_count = ((len(palette) + 15) // 16) * 16

    if colors_count > 240:
        raise SystemExit('%d colors leave no sprite palette for the text' % len(palette))

    palette += [(0, 0, 0)] * (colors_count - len(palette))
    index_of = {color: index + 1 for index, color in enumerate(colors)}

    # The logo centered in the grid.
    x_offset = (grid_width - logo_width) // 2
    y_offset = (grid_height - logo_height) // 2
    grid = [[0] * grid_width for _ in range(grid_height)]

    for y in range(logo_height):
        for x in range(logo_width):
            color = pixels[(top + y) * width + left + x]

            if color:
                grid[y_offset + y][x_offset + x] = index_of[color]

    # One frame under the other, bottom row first as BMP wants.
    rows = []

    for grid_row in range(GRID_ROWS):
        for grid_column in range(GRID_COLUMNS):
            for y in range(SPRITE_SIZE):
                x = grid_column * SPRITE_SIZE
                rows.append(bytes(grid[grid_row * SPRITE_SIZE + y][x:x + SPRITE_SIZE]))

    data = b''.join(reversed(rows))
    offset = 14 + 40 + colors_count * 4

    with open(os.path.join(GRAPHICS, 'title_logo.bmp'), 'wb') as bmp:
        bmp.write(struct.pack('<2sIHHI', b'BM', offset + len(data), 0, 0, offset))
        bmp.write(struct.pack('<IiiHHIIiiII', 40, SPRITE_SIZE, len(rows), 1, 8, 0, len(data),
                              2834, 2834, colors_count, colors_count))

        for r, g, b in palette:
            bmp.write(struct.pack('<BBBB', b, g, r, 0))

        bmp.write(data)

    with open(os.path.join(GRAPHICS, 'title_logo.json'), 'w') as json_file:
        json_file.write('{\n    "type": "sprite",\n    "height": %d\n}\n' % SPRITE_SIZE)

    print('title_logo.bmp %dx%d logo, %d colors in a %d color palette'
          % (logo_width, logo_height, len(colors), colors_count))


if __name__ == '__main__':
    main()
