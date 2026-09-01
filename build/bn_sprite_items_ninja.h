#ifndef BN_SPRITE_ITEMS_NINJA_H
#define BN_SPRITE_ITEMS_NINJA_H

#include "bn_sprite_item.h"

//{{BLOCK(ninja_bn_gfx)

//======================================================================
//
//	ninja_bn_gfx, 16x256@4, 
//	+ palette 16 entries, not compressed
//	+ 64 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 2048 = 2080
//
//	Time-stamp: 2026-08-31, 16:05:07
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_NINJA_BN_GFX_H
#define GRIT_NINJA_BN_GFX_H

#define ninja_bn_gfxTilesLen 2048
extern const bn::tile ninja_bn_gfxTiles[64];

#define ninja_bn_gfxPalLen 32
extern const bn::color ninja_bn_gfxPal[16];

#endif // GRIT_NINJA_BN_GFX_H

//}}BLOCK(ninja_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item ninja(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(ninja_bn_gfxTiles, 64), bpp_mode::BPP_4, compression_type::NONE, 16), 
            sprite_palette_item(span<const color>(ninja_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

