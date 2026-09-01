
@{{BLOCK(card_back_bn_gfx)

@=======================================================================
@
@	card_back_bn_gfx, 16x16@4, 
@	+ palette 16 entries, not compressed
@	+ 4 tiles Metatiled by 2x2 not compressed
@	Total size: 32 + 128 = 160
@
@	Time-stamp: 2026-08-31, 15:57:44
@	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global card_back_bn_gfxTiles		@ 128 unsigned chars
	.hidden card_back_bn_gfxTiles
card_back_bn_gfxTiles:
	.word 0x11111110,0x23232321,0x23232321,0x32323231,0x32323231,0x23232321,0x23232321,0x32323231
	.word 0x01111111,0x13232323,0x13232323,0x12323232,0x12323232,0x13232323,0x13232323,0x12323232
	.word 0x32323231,0x23232321,0x23232321,0x32323231,0x32323231,0x23232321,0x23232321,0x11111110
	.word 0x12323232,0x13232323,0x13232323,0x12323232,0x12323232,0x13232323,0x13232323,0x01111111

	.section .rodata
	.align	2
	.global card_back_bn_gfxPal		@ 32 unsigned chars
	.hidden card_back_bn_gfxPal
card_back_bn_gfxPal:
	.hword 0x03E0,0x0000,0x001F,0x7FFF,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(card_back_bn_gfx)
