/*
 * gfx.c
 *
 *  Created on: 09.07.2018
 *      Author: konrad
 */

#include "gfx/gfx.h"
#include "gfx/gfx_lut.h"

void gfx_set_pixel(int line, int column, int color) {
	int page = line >> 3;
	line &= 0b111;	// modulo 8
	line <<= 1;
	gfx_buffer[page][column] &= (~(0b11 << line));	// Erase pixel
	gfx_buffer[page][column] |= (color << line);	// Set pixel color
}

void gfx_clear_buffer(void) {
	// TODO Can work quicker (zeroing using uint32_t)
	for (int p=0; p<OLED_PAGES; ++p) {
		for (int c=0; c<OLED_WIDTH; ++c) {
			gfx_buffer[p][c] = 0x0000;
		}
	}
}

void gfx_update(void) {
	// TODO make lookup table - will work wuicker
	for (int p=0; p<OLED_PAGES; ++p) {
		for (int c=0; c<OLED_WIDTH; ++c) {
//			uint8_t res[3] = {0, 0, 0};
//			for (int i=0; i<8; ++i) {
//				uint16_t mask = (0b11 << (i*2));
//				mask &= gfx_buffer[p][c];
//				mask >>= (i*2);
//				if (mask&1) {
//					res[1] |= (1 << i);
//				}
//				if (mask&2) {
//					res[0] |= (1 << i);
//					res[2] |= (1 << i);
//				}
//			}
			for (int i=0; i<3; ++i) {
				uint8_t res = 0;

				res |= gfx_lut[(i+c-p+9)%3][gfx_buffer[p][c]&0xFF];
				res |= gfx_lut[(i+1+c-p+9)%3][(gfx_buffer[p][c]>>8)&0xFF] << 4;
				oled_triple_buffer[i][p][c] = res;
			}
		}
	}
}
