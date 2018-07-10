/*
 * gfx.c
 *
 *  Created on: 09.07.2018
 *      Author: konrad
 */

#include "gfx/gfx.h"
#include "gfx/gfx_lut.h"

uint16_t gfx_buffer[OLED_PAGES][OLED_WIDTH];
uint32_t gfx_clear_color = 0x00000000;

void gfx_clear(void) {
	// Using 32-bit pointers to make it faster
	register uint32_t *end = &(gfx_buffer[OLED_PAGES][OLED_WIDTH]);
	for (uint32_t* p = (uint32_t*) &gfx_buffer; p < end; ++p) {
		*p = gfx_clear_color;
	}
}

void gfx_update(void) {
	for (int p=0; p<OLED_PAGES; ++p) {
		for (int c=0; c<OLED_WIDTH; ++c) {
			for (int i=0; i<3; ++i) {
				register uint8_t res = 0;
				res |= gfx_lut[(i+c-p+9)%3][gfx_buffer[p][c]&0xFF];
				res |= gfx_lut[(i+1+c-p+9)%3][(gfx_buffer[p][c]>>8)&0xFF] << 4;
				oled_triple_buffer[i][p][c] = res;
			}
		}
	}
}

void gfx_set_clear_color(int color) {
	switch (color) {
	case COLOR_WHITE:
		gfx_clear_color = 0xFFFFFFFF;
		break;
	case COLOR_LIGHTGRAY:
		gfx_clear_color = 0xAAAAAAAA;
		break;
	case COLOR_DARKGRAY:
		gfx_clear_color = 0x55555555;
		break;
	default:
		gfx_clear_color = 0x00000000;
		break;
	}
}

void gfx_set_pixel(int x, int y, int color) {
	int page = y >> 3;
	y &= 0b111;	// modulo 8
	y <<= 1;
	gfx_buffer[page][x] &= (~(0b11 << y));	// Erase pixel
	gfx_buffer[page][x] |= (color << y);	// Set pixel color
}
