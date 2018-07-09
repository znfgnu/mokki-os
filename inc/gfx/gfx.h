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

uint16_t gfx_buffer[OLED_PAGES][OLED_WIDTH];

void gfx_set_pixel(int line, int column, int color);
void gfx_clear_buffer(void);
void gfx_update(void);

#endif /* GFX_GFX_H_ */
