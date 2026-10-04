"""
Builds the title screen's "PRESS <START button>" prompt as one sprite.

The prompt sits over the city, where plain text gets lost, so it needs a dark
border. The game font has none, so instead of using text sprites the word is
rendered here from the same TTF, put next to the START button animation and
the whole line gets a 2px outline (thicker than the 1px that fits in the
game font's own cells).

The button source is a 32x16 PNG, two 16x16 frames: released and pressed.

Outputs:
    graphics/press_start.bmp    64x64 4bpp, two 64x32 frames
    graphics/press_start.json   butano sprite item descriptor

Usage: python scripts/make_press_start.py
"""

import os
import struct

from PIL import Image, ImageDraw, ImageFont

TTF = '/Users/rodrigocasale/Documents/proyects/gba_dev/juniors/madspixel/madspixel.ttf'
BUTTON = '/Users/rodrigocasale/Documents/proyects/gba_dev/juniors/start_button.png'

TEXT = 'PRESS'
PIXEL_SIZE = 16
GAP = 1
BORDER = 2
BUTTON_SIZE = 16
FRAMES = 2

SPRITE_W = 64
SPRITE_H = 32

TRANSPARENT = (0, 255, 0)
OUTLINE = (20, 14, 30)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')


def render_text(color):
    font = ImageFont.truetype(TTF, PIXEL_SIZE)
    mask = Image.new('L', (SPRITE_W, SPRITE_H), 0)
    draw = ImageDraw.Draw(mask)
    draw.fontmode = '1'  # no antialiasing: this is a pixel font
    draw.text((0, 0), TEXT, font=font, fill=255)
    mask = mask.crop(mask.getbbox())

    text = Image.new('RGBA', mask.size, (0, 0, 0, 0))
    text.paste(color + (255,), (0, 0), mask)
    return text


def outlined(line):
    """Returns the line on a canvas BORDER bigger on every side, with a border around it."""
    canvas = Image.new('RGBA', (line.width + BORDER * 2, line.height + BORDER * 2), (0, 0, 0, 0))
    canvas.paste(line, (BORDER, BORDER))
    pixels = canvas.load()

    # Every offset within BORDER, minus the far corners so the border is rounded.
    offsets = [(dx, dy) for dx in range(-BORDER, BORDER + 1) for dy in range(-BORDER, BORDER + 1)
               if BORDER == 1 or abs(dx) + abs(dy) < BORDER * 2]
    border = []

    for y in range(canvas.height):
        for x in range(canvas.width):
            if not pixels[x, y][3]:
                neighbors = ((x + dx, y + dy) for dx, dy in offsets)

                if any(0 <= nx < canvas.width and 0 <= ny < canvas.height and pixels[nx, ny][3]
                       for nx, ny in neighbors):
                    border.append((x, y))

    for x, y in border:
        pixels[x, y] = OUTLINE + (255,)

    return canvas


def main():
    button = Image.open(BUTTON).convert('RGBA')
    button_colors = sorted({pixel[:3] for pixel in button.getdata() if pixel[3]}, key=sum)
    text = render_text(button_colors[-1])  # the button's light color

    height = max(text.height, BUTTON_SIZE)
    frames = []

    for frame in range(FRAMES):
        line = Image.new('RGBA', (text.width + GAP + BUTTON_SIZE, height), (0, 0, 0, 0))
        line.paste(text, (0, (height - text.height) // 2))
        line.paste(button.crop((frame * BUTTON_SIZE, 0, (frame + 1) * BUTTON_SIZE, BUTTON_SIZE)),
                   (text.width + GAP, (height - BUTTON_SIZE) // 2))
        frames.append(outlined(line))

    width, height = frames[0].size

    if width > SPRITE_W or height > SPRITE_H:
        raise SystemExit('the %dx%d prompt does not fit a %dx%d sprite' % (width, height, SPRITE_W, SPRITE_H))

    palette = [TRANSPARENT] + sorted({pixel[:3] for frame in frames for pixel in frame.getdata() if pixel[3]})
    palette += [(0, 0, 0)] * (16 - len(palette))
    x_offset = (SPRITE_W - width) // 2
    y_offset = (SPRITE_H - height) // 2
    rows = []

    for frame in frames:
        pixels = frame.load()

        for y in range(SPRITE_H):
            row = bytearray(SPRITE_W // 2)

            for x in range(SPRITE_W):
                source_x = x - x_offset
                source_y = y - y_offset

                if 0 <= source_x < width and 0 <= source_y < height and pixels[source_x, source_y][3]:
                    row[x // 2] |= palette.index(pixels[source_x, source_y][:3]) << (0 if x % 2 else 4)

            rows.append(bytes(row))

    data = b''.join(reversed(rows))
    offset = 14 + 40 + 16 * 4

    with open(os.path.join(GRAPHICS, 'press_start.bmp'), 'wb') as bmp:
        bmp.write(struct.pack('<2sIHHI', b'BM', offset + len(data), 0, 0, offset))
        bmp.write(struct.pack('<IiiHHIIiiII', 40, SPRITE_W, len(rows), 1, 4, 0, len(data), 2834, 2834, 16, 16))

        for r, g, b in palette:
            bmp.write(struct.pack('<BBBB', b, g, r, 0))

        bmp.write(data)

    with open(os.path.join(GRAPHICS, 'press_start.json'), 'w') as json_file:
        json_file.write('{\n    "type": "sprite",\n    "height": %d\n}\n' % SPRITE_H)

    print('press_start.bmp %dx%d prompt in %dx%d frames' % (width, height, SPRITE_W, SPRITE_H))


if __name__ == '__main__':
    main()
