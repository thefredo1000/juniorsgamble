#ifndef BN_SPRITE_ITEMS_CARD_BACK_H
#define BN_SPRITE_ITEMS_CARD_BACK_H

#include "bn_sprite_item.h"

//{{BLOCK(card_back_bn_gfx)

//======================================================================
//
//	card_back_bn_gfx, 16x16@4, 
//	+ palette 16 entries, not compressed
//	+ 4 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 128 = 160
//
//	Time-stamp: 2026-08-31, 15:57:44
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CARD_BACK_BN_GFX_H
#define GRIT_CARD_BACK_BN_GFX_H

#define card_back_bn_gfxTilesLen 128
extern const bn::tile card_back_bn_gfxTiles[4];

#define card_back_bn_gfxPalLen 32
extern const bn::color card_back_bn_gfxPal[16];

#endif // GRIT_CARD_BACK_BN_GFX_H

//}}BLOCK(card_back_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item card_back(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(card_back_bn_gfxTiles, 4), bpp_mode::BPP_4, compression_type::NONE, 1), 
            sprite_palette_item(span<const color>(card_back_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

