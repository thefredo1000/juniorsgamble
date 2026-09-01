#ifndef BN_REGULAR_BG_ITEMS_JUNIORS_H
#define BN_REGULAR_BG_ITEMS_JUNIORS_H

#include "bn_regular_bg_item.h"

//{{BLOCK(juniors_bn_gfx)

//======================================================================
//
//	juniors_bn_gfx, 256x256@8, 
//	+ palette 128 entries, not compressed
//	+ 347 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 256 + 22208 + 2048 = 24512
//
//	Time-stamp: 2026-08-31, 15:57:44
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_JUNIORS_BN_GFX_H
#define GRIT_JUNIORS_BN_GFX_H

#define juniors_bn_gfxTilesLen 22208
extern const bn::tile juniors_bn_gfxTiles[694];

#define juniors_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell juniors_bn_gfxMap[1024];

#define juniors_bn_gfxPalLen 256
extern const bn::color juniors_bn_gfxPal[128];

#endif // GRIT_JUNIORS_BN_GFX_H

//}}BLOCK(juniors_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item juniors(
            regular_bg_tiles_item(span<const tile>(juniors_bn_gfxTiles, 694), bpp_mode::BPP_8, compression_type::NONE), 
            bg_palette_item(span<const color>(juniors_bn_gfxPal, 128), bpp_mode::BPP_8, compression_type::NONE),
            regular_bg_map_item(juniors_bn_gfxMap[0], size(32, 32), compression_type::NONE, 1, false));
}

#endif

