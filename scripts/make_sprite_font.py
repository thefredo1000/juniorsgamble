"""
Renders a TTF into a Butano variable-width sprite font.

Butano sprite fonts are a 4bpp BMP with one glyph per cell, cells stacked
vertically starting at ASCII 33 ('!'), followed by the extra UTF-8 glyphs.
Space (32) has no cell, only a width. Cell size must be a valid sprite size;
variable-width text only supports 8x8, 8x16 and 16x16 (see
bn_sprite_text_generator.cpp), and madspixel is designed on a 16px body, so
this uses 16x16.

Outputs:
    graphics/<name>.bmp        glyph sheet (4bpp, palette index 0 transparent)
    graphics/<name>.json       butano sprite item descriptor
    include/<name>_sprite_font.h   bn::sprite_font with the width table

Usage: python scripts/make_sprite_font.py
"""

import os
import struct

from PIL import Image, ImageDraw, ImageFont

TTF = '/Users/rodrigocasale/Documents/proyects/gba_dev/juniors/madspixel/madspixel.ttf'
NAME = 'madspixel_font'
FONT_ID = 'madspixel'
PIXEL_SIZE = 16
CELL_W = 16
CELL_H = 16

# Same extra glyphs butano's common fonts carry, so Spanish team names
# ("America", "Juarez") keep their accents. madspixel only ships ASCII plus a
# few symbols, so everything below except the inverted exclamation mark is
# composed here from a base glyph and a hand-drawn mark.
UTF8_CHARS = list('ÁÉÍÓÚÜÑáéíóúüñ¡¿')
ASCII_CHARS = [chr(c) for c in range(33, 127)]

# Row layout of the rendered glyphs at this size: caps occupy rows 1..11,
# lowercase x-height starts at row 4, the baseline is row 12 and descenders
# run to row 15. Lowercase therefore has room for a mark as-is; caps have to
# drop into the (unused) descender rows to make space above them.
CAP_ACCENT_DROP = 2
LOWERCASE_ACCENT_ROW = 1
X_HEIGHT_ROW = 4

# base glyph, mark
COMPOSED = {
    'Á': ('A', 'acute'), 'É': ('E', 'acute'), 'Í': ('I', 'acute'),
    'Ó': ('O', 'acute'), 'Ú': ('U', 'acute'), 'Ü': ('U', 'diaeresis'),
    'Ñ': ('N', 'tilde'),
    'á': ('a', 'acute'), 'é': ('e', 'acute'), 'í': ('i', 'acute'),
    'ó': ('o', 'acute'), 'ú': ('u', 'acute'), 'ü': ('u', 'diaeresis'),
    'ñ': ('n', 'tilde'),
}

# Marks as (column offset, row offset) pairs, drawn from the top left of the
# mark's own box; the box is then centered over the base glyph's ink.
MARKS = {
    'acute':     ((1, 0), (0, 1)),
    'diaeresis': ((0, 1), (2, 1)),
    'tilde':     ((1, 0), (2, 0), (4, 0), (0, 1), (3, 1), (4, 1)),
}

# Every glyph gets a 1px dark border so text reads over any background. That
# is as thick as it can go: variable width text needs 16x16 cells and the font
# already uses all 16 rows, so even 1px only fits by taking one row off the
# tallest ascenders and descenders.
BORDER = 1
TEXT_COLOR = (255, 255, 255)
BORDER_COLOR = (20, 14, 30)

PROJECT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def write_bmp_4bpp(path, cells, width, height, palette):
    """Writes a bottom-up 4bpp BMP with a 40 byte header (butano's bmp.py
    rejects anything else)."""
    row_bytes = width // 2
    assert row_bytes % 4 == 0, 'row padding not implemented'
    pixels_offset = 14 + 40 + 16 * 4
    image_size = row_bytes * height
    out = bytearray()
    out += b'BM'
    out += struct.pack('<IHHI', pixels_offset + image_size, 0, 0, pixels_offset)
    out += struct.pack('<IiiHHIIiiII', 40, width, height, 1, 4, 0, image_size, 0, 0, 16, 16)

    for i in range(16):
        r, g, b = palette[i] if i < len(palette) else (0, 0, 0)
        out += struct.pack('<BBBB', b, g, r, 0)

    for y in reversed(range(height)):  # bottom-up
        row = cells[y]
        for x in range(0, width, 2):
            out.append((row[x] << 4) | row[x + 1])

    with open(path, 'wb') as bmp_file:
        bmp_file.write(bytes(out))


def remove_row(cell, row, from_top):
    """Deletes a row, closing the gap with the rows above it (from_top) or below it."""
    if from_top:
        moved = cell.crop((0, 0, CELL_W, row))
        cell.paste(0, (0, 0, CELL_W, row + 1))
        cell.paste(moved, (0, 1))
    else:
        moved = cell.crop((0, row + 1, CELL_W, CELL_H))
        cell.paste(0, (0, row, CELL_W, CELL_H))
        cell.paste(moved, (0, row))


def make_room_for_border(cell):
    """Keeps the ink off the first and last BORDER rows, so the border fits there.

    A glyph that reaches an edge loses a row of its ascender or descender:
    one that repeats the row next to it, so a stem gets shorter and nothing
    else changes. A glyph with no such row and room to spare is moved instead.
    """
    def row_pixels(y):
        return list(cell.crop((0, y, CELL_W, y + 1)).getdata())

    for from_top in (True, False):
        for _ in range(BORDER):
            bbox = cell.getbbox()

            if not bbox:
                return

            if from_top:
                if bbox[1] >= BORDER:
                    break

                candidates = [y for y in range(1, 5) if row_pixels(y) == row_pixels(y - 1)]

                if candidates:
                    remove_row(cell, candidates[0], True)
                elif bbox[3] < CELL_H - BORDER:
                    cell.paste(cell.copy(), (0, 1))
                    cell.paste(0, (0, 0, CELL_W, 1))
                else:
                    remove_row(cell, 1, True)
            else:
                if bbox[3] <= CELL_H - BORDER:
                    break

                candidates = [y for y in range(CELL_H - 2, CELL_H - 6, -1)
                              if row_pixels(y) == row_pixels(y + 1)]

                if candidates:
                    remove_row(cell, candidates[0], False)
                elif bbox[1] > BORDER:
                    moved = cell.crop((0, 1, CELL_W, CELL_H))
                    cell.paste(0, (0, 0, CELL_W, CELL_H))
                    cell.paste(moved, (0, 0))
                else:
                    remove_row(cell, CELL_H - 2, False)


def with_border(cell):
    """Returns the cell's rows of palette indexes: 1 text, 2 border, 0 transparent."""
    make_room_for_border(cell)
    pixels = cell.load()

    def ink(x, y):
        # The glyph moves right to leave room for the border on its left.
        x -= BORDER
        return 0 <= x < CELL_W and 0 <= y < CELL_H and pixels[x, y]

    rows = []

    for y in range(CELL_H):
        row = []

        for x in range(CELL_W):
            if ink(x, y):
                row.append(1)
            elif any(ink(x + dx, y + dy) for dx in range(-BORDER, BORDER + 1)
                     for dy in range(-BORDER, BORDER + 1)):
                row.append(2)
            else:
                row.append(0)

        rows.append(row)

    return rows


def main():
    font = ImageFont.truetype(TTF, PIXEL_SIZE)
    ascent, descent = font.getmetrics()
    assert ascent + descent <= CELL_H, (ascent, descent)

    chars = ASCII_CHARS + UTF8_CHARS
    rows = []
    widths = [int(round(font.getlength(' ')))]

    def render(char):
        cell = Image.new('L', (CELL_W, CELL_H), 0)
        draw = ImageDraw.Draw(cell)
        draw.fontmode = '1'  # no antialiasing: this is a pixel font
        draw.text((0, 0), char, font=font, fill=255)  # y=0 is the ascender line
        return cell

    def compose(char):
        """Builds an accented glyph out of a base glyph plus a mark."""
        base_char, mark = COMPOSED[char]
        base = render(base_char)
        upper = base_char.isupper()

        if upper:
            base = base.transform(base.size, Image.AFFINE, (1, 0, 0, 0, 1, -CAP_ACCENT_DROP))
            mark_row = LOWERCASE_ACCENT_ROW - 1
        else:
            # Drops the dot of "i" so "í" doesn't end up with both.
            base.paste(0, (0, 0, CELL_W, X_HEIGHT_ROW))
            mark_row = LOWERCASE_ACCENT_ROW

        bbox = base.getbbox()
        assert bbox, char
        offsets = MARKS[mark]
        mark_width = max(x for x, _ in offsets) + 1
        mark_x = bbox[0] + ((bbox[2] - bbox[0] - mark_width) // 2)
        assert mark_x >= 0 and mark_x + mark_width <= CELL_W, (char, mark_x)

        pixels = base.load()
        for x, y in offsets:
            pixels[mark_x + x, mark_row + y] = 255

        return base

    for char in chars:
        if char in COMPOSED:
            cell = compose(char)
            advance = int(round(font.getlength(COMPOSED[char][0])))
        elif char == '¿':
            # Not in the font either: the mirrored question mark, kept on the
            # same rows so it sits on the baseline like "?" does.
            cell = render('?')
            bbox = cell.getbbox()
            band = cell.crop(bbox).transpose(Image.ROTATE_180)
            cell.paste(0, (0, 0, CELL_W, CELL_H))
            cell.paste(band, bbox)
            advance = int(round(font.getlength('?')))
        else:
            cell = render(char)
            advance = int(round(font.getlength(char)))

        bbox = cell.getbbox()
        if bbox:
            assert bbox[2] + BORDER * 2 <= CELL_W and bbox[3] <= CELL_H, (char, bbox)

        rows += with_border(cell)

        # The font's own advance leaves 1px after the ink, which is where the
        # right border goes; the left border is what makes the glyph wider.
        advance += BORDER
        assert 0 < advance <= CELL_W, (char, advance)
        widths.append(advance)

    palette = [(0, 255, 0), TEXT_COLOR, BORDER_COLOR]  # index 0 is the transparent one
    bmp_path = os.path.join(PROJECT, 'graphics', NAME + '.bmp')
    write_bmp_4bpp(bmp_path, rows, CELL_W, len(rows), palette)

    with open(os.path.join(PROJECT, 'graphics', NAME + '.json'), 'w') as json_file:
        json_file.write('{\n    "type": "sprite",\n    "height": %d\n}\n' % CELL_H)

    guard = FONT_ID.upper() + '_SPRITE_FONT_H'
    labels = [chr(c) for c in range(32, 127)] + UTF8_CHARS
    lines = []
    for i, width in enumerate(widths):
        code = 32 + i if i < 95 else None
        label = labels[i]

        if label == '\\':
            label = ''  # a trailing backslash would line-continue the comment

        comment = '%d %s' % (code, label) if code is not None else label
        lines.append('        %d,  // %s' % (width, comment.rstrip()))

    header = '''// Generated by scripts/make_sprite_font.py from madspixel.ttf. Do not edit by hand.

#ifndef {guard}
#define {guard}

#include "bn_sprite_font.h"
#include "bn_utf8_characters_map.h"
#include "bn_sprite_items_{name}.h"

namespace Game
{{
    constexpr bn::utf8_character {id}_sprite_font_utf8_characters[] = {{
        {utf8}
    }};

    constexpr int8_t {id}_sprite_font_character_widths[] = {{
{widths}
    }};

    constexpr bn::span<const bn::utf8_character> {id}_sprite_font_utf8_characters_span(
            {id}_sprite_font_utf8_characters);

    constexpr auto {id}_sprite_font_utf8_characters_map =
            bn::utf8_characters_map<{id}_sprite_font_utf8_characters_span>();

    constexpr bn::sprite_font {id}_sprite_font(
            bn::sprite_items::{name}, {id}_sprite_font_utf8_characters_map.reference(),
            {id}_sprite_font_character_widths);
}}

#endif // {guard}
'''.format(guard=guard, name=NAME, id=FONT_ID,
           utf8=', '.join('"%s"' % c for c in UTF8_CHARS),
           widths='\n'.join(lines))

    with open(os.path.join(PROJECT, 'include', FONT_ID + '_sprite_font.h'), 'w') as header_file:
        header_file.write(header)

    print('%s: %d glyphs, %dx%d cells, ascent %d, widths %d..%d' %
          (NAME, len(chars), CELL_W, CELL_H, ascent, min(widths), max(widths)))


if __name__ == '__main__':
    main()
