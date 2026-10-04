"""
Converts the "Televisa presenta" card into the background of the boot screen.

The source is a 240x160 true color PNG. The GBA only has 15 bit color, so it
is reduced to that first, and then to the 255 colors an 8bpp background can
show (palette index 0 is the transparent one and is left unused).

Outputs:
    graphics/presenter.bmp    256x256 8bpp, the card in the top left corner
    graphics/presenter.json   butano regular bg descriptor

Usage: python scripts/make_presenter_bg.py
"""

import os
import struct

from PIL import Image

SOURCE = '/Users/rodrigocasale/Documents/photos/edited/TELEVISA.png'

BG_SIZE = 256
TRANSPARENT = (255, 0, 255)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPHICS = os.path.join(ROOT, 'graphics')


def main():
    source = Image.open(SOURCE).convert('RGB')

    if source.size != (240, 160):
        raise SystemExit('%s is %dx%d, expected 240x160' % ((SOURCE,) + source.size))

    # What the GBA would show anyway: 5 bits per channel.
    source = source.point(lambda value: value & 0xF8)
    quantized = source.quantize(colors=255, method=Image.MEDIANCUT, dither=Image.NONE)
    colors = quantized.getpalette()[:255 * 3]
    palette = [TRANSPARENT] + [tuple(colors[index:index + 3]) for index in range(0, len(colors), 3)]
    palette += [(0, 0, 0)] * (256 - len(palette))
    pixels = quantized.load()
    fill = pixels[0, 0] + 1
    rows = []

    for y in range(BG_SIZE):
        if y < source.height:
            row = [pixels[x, y] + 1 for x in range(source.width)]
            rows.append(bytes(row + [fill] * (BG_SIZE - source.width)))
        else:
            rows.append(bytes([fill] * BG_SIZE))

    data = b''.join(reversed(rows))
    offset = 14 + 40 + 256 * 4

    with open(os.path.join(GRAPHICS, 'presenter.bmp'), 'wb') as bmp:
        bmp.write(struct.pack('<2sIHHI', b'BM', offset + len(data), 0, 0, offset))
        bmp.write(struct.pack('<IiiHHIIiiII', 40, BG_SIZE, BG_SIZE, 1, 8, 0, len(data), 2834, 2834, 256, 256))

        for r, g, b in palette:
            bmp.write(struct.pack('<BBBB', b, g, r, 0))

        bmp.write(data)

    with open(os.path.join(GRAPHICS, 'presenter.json'), 'w') as json_file:
        json_file.write('{\n    "type": "regular_bg"\n}\n')

    print('presenter.bmp %d colors' % len(set(quantized.getdata())))


if __name__ == '__main__':
    main()
