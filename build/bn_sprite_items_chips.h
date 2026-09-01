#ifndef BN_SPRITE_ITEMS_CHIPS_H
#define BN_SPRITE_ITEMS_CHIPS_H

#include "bn_sprite_item.h"

//{{BLOCK(chips_bn_gfx)

//======================================================================
//
//	chips_bn_gfx, 16x96@4, 
//	+ palette 16 entries, not compressed
//	+ 24 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 768 = 800
//
//	Time-stamp: 2026-08-31, 15:57:44
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CHIPS_BN_GFX_H
#define GRIT_CHIPS_BN_GFX_H

#define chips_bn_gfxTilesLen 768
extern const bn::tile chips_bn_gfxTiles[24];

#define chips_bn_gfxPalLen 32
extern const bn::color chips_bn_gfxPal[16];

#endif // GRIT_CHIPS_BN_GFX_H

//}}BLOCK(chips_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item chips(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(chips_bn_gfxTiles, 24), bpp_mode::BPP_4, compression_type::NONE, 6), 
            sprite_palette_item(span<const color>(chips_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

