/*
 * gfx.h
 *
 *  Created on: 08.07.2018
 *      Author: konrad
 */

#ifndef GFX_GFX_H_
#define GFX_GFX_H_

#include "hw/oled.h"

/*
 * 4-color buffer
 *
 * When gfx_update() is invoked, buffer is copied to
 * OLED 3-framed buffer.
 */

#define COLOR_BLACK		0b00
#define COLOR_DARKGRAY	0b01
#define COLOR_LIGHTGRAY	0b10
#define COLOR_WHITE		0b11

extern uint16_t gfx_buffer[OLED_PAGES][OLED_WIDTH];
extern uint32_t gfx_clear_color;

typedef struct {
	/*
	 * There are three supported types of textures:
	 * 1. Rectangle-shaped non-transparent sprite (4 colors)
	 * 2. Transparent sprite with 4 colors (color data followed by transparency
	 *    data)
	 * 3. Transparent sprite with palette of 3 colors (11b is transparent)
	 * 4. Transparent sprite with one color used as transparency
	 *
	 * Byte number | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
	 * ------------+---+---+---+---+---+---+---+---
	 * Type 1      | 0 | 0 | x | x | x | x | x | x
	 * Type 2      | 0 | 1 | x | x | x | x | x | x
	 * Type 3      | 1 | 0 | a | a | b | b | c | c
	 * Type 4      | 1 | 1 | x | x | x | x | d | d
	 *
	 * Where:
	 *   x - should be zero, but don't care for now
	 *   aa - (3rd type) palette color for 10b
	 *   bb - (3rd type) palette color for 01b
	 *   cc - (3rd type) palette color for 00b
	 *   dd - (4th type) color used as transparency
	 */

	uint8_t header;
	uint8_t extra;	// ???
	uint8_t width;
	uint8_t height;
	uint16_t data[];
} sprite_t;

#define SPRITE_TYPE_GET(x)	((x) & 0xC0)
#define SPRITE_TYPE_1		0x00
#define SPRITE_TYPE_2		0x40
#define SPRITE_TYPE_3		0x80
#define SPRITE_TYPE_4		0xC0

void gfx_clear(void);
void gfx_update(void);
void gfx_set_clear_color(int color);
void gfx_set_pixel(int x, int y, int color);
void gfx_blit(sprite_t* sprite, int x, int y);

#endif /* GFX_GFX_H_ */
