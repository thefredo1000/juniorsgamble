#ifndef BN_AFFINE_BG_ITEMS_LAND_H
#define BN_AFFINE_BG_ITEMS_LAND_H

#include "bn_affine_bg_item.h"

//{{BLOCK(land_bn_gfx)

//======================================================================
//
//	land_bn_gfx, 4096x4096@8, 
//	+ palette 80 entries, not compressed
//	+ 225 tiles (t reduced) not compressed
//	+ affine map, not compressed, 512x512 
//	Total size: 160 + 14400 + 262144 = 276704
//
//	Time-stamp: 2026-08-31, 16:05:08
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_LAND_BN_GFX_H
#define GRIT_LAND_BN_GFX_H

#define land_bn_gfxTilesLen 14400
extern const bn::tile land_bn_gfxTiles[450];

#define land_bn_gfxMapLen 262144
extern const bn::affine_bg_map_cell land_bn_gfxMap[262144];

#define land_bn_gfxPalLen 160
extern const bn::color land_bn_gfxPal[80];

#endif // GRIT_LAND_BN_GFX_H

//}}BLOCK(land_bn_gfx)

namespace bn::affine_bg_items
{
    constexpr inline affine_bg_item land(
            affine_bg_tiles_item(span<const tile>(land_bn_gfxTiles, 450), compression_type::NONE), 
            bg_palette_item(span<const color>(land_bn_gfxPal, 80), bpp_mode::BPP_8, compression_type::NONE),
            affine_bg_map_item(land_bn_gfxMap[0], size(512, 512), compression_type::NONE, 1, true));
}

#endif

