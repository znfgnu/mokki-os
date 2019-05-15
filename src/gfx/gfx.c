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
	while(oled_request_flip);
	for (int p=0; p<OLED_PAGES; ++p) {
		for (int c=0; c<OLED_WIDTH; ++c) {
			for (int i=0; i<3; ++i) {	// Color iteration
				// Lookup table used instead of bitmagic
				register uint8_t res = 0;
				res |= gfx_lut[(i+c-p+9)%3][gfx_buffer[p][c]&0xFF];
				res |= gfx_lut[(i+1+c-p+9)%3][(gfx_buffer[p][c]>>8)&0xFF] << 4;
				oled_triple_buffer[oled_free_buffer][i][p][c] = res;
			}
		}
	}
	oled_request_flip = 1;
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
	if (x & ~(0x7F)) return;
	if (y & ~(0x3F)) return;
	int page = y >> 3;
	y &= 0b111;	// modulo 8
	y <<= 1;
	gfx_buffer[page][x] &= (~(0b11 << y));	// Erase pixel
	gfx_buffer[page][x] |= (color << y);	// Set pixel color
}

void gfx_blit(sprite_t *sprite, int x, int y) {
	int page_start = y / 8;
	int page_end = (y + sprite->height + 7) / 8;
	if (page_end > OLED_PAGES) {
		page_end = OLED_PAGES;
	}
	int page_total = page_end - page_start;

	int col_start = x;
	int col_end = x + sprite->width;
	if (col_end > OLED_WIDTH) {
		col_end = OLED_WIDTH;
	}
	int col_total = col_end - col_start;

	int offset = (y & 7) * 2;	// how many bits from previous line
	int last_line_lines;
	if (y + sprite->height >= OLED_HEIGHT) {
		last_line_lines = 16;
	} else {
		last_line_lines = ((y + sprite->height) & 7) * 2;
		if (last_line_lines == 0) last_line_lines = 16;
	}

	uint8_t *after_data = &(sprite->data[sprite->width * ((sprite->height + 7)/8)]);

//	if (offset == 0) {
//		// no offset, no cry!
//	} else {
		// there is in-page offset, deal with it.
		uint16_t current_line_mask = (1 << (16-offset)) - 1;	// applied to sprite
		uint16_t previous_line_mask = ~current_line_mask;
		for (int p = 0; p < page_total; ++p) {
			for (int c = 0; c < col_total; ++c) {
				uint16_t* buffer_place = &gfx_buffer[page_start + p][col_start + c];
				uint16_t* current_line_place = &(sprite->data[p*sprite->width + c]);
				uint16_t* previous_line_place = &(sprite->data[(p-1)*sprite->width + c]);

				uint16_t buffer_candidate = 0x0000;
				uint16_t buffer_mask = 0xFFFF;

				// TYPE2 variables
				uint8_t previous_trans = after_data[(p-1)*sprite->width + c];
				uint8_t current_trans = after_data[p*sprite->width + c];
				uint16_t previous_trans_16 = 0x0000;
				uint16_t current_trans_16 = 0x0000;

				// Depending on sprite type, generate buffer_candidate and
				// buffer_mask, which will be saved to gfx_buffer (masked).
				switch(SPRITE_TYPE_GET(sprite->header)) {
				case SPRITE_TYPE_1:	// no transparency
					if (p == 0) {	// if first page, there is no previous page
						buffer_mask &= ~((1 << offset) - 1);
					} else {		// else, get some pixels from previous page
						buffer_candidate |= (*previous_line_place & previous_line_mask) >> (16 - offset);
					}

					if (p + 1 == page_total) {
						if (last_line_lines > offset) {	// if last page in loop and current line exists
							buffer_candidate |= (*current_line_place & current_line_mask) << offset;
						}
						buffer_mask &= (1 << last_line_lines) - 1;
					} else {
						buffer_candidate |= (*current_line_place & current_line_mask) << offset;
					}
					break;
				case SPRITE_TYPE_2:
					for (int i=0; i<8; ++i) {
						uint16_t mask = (1<<(2*i));
						mask |= mask<<1;
						if (previous_trans & (1<<i)) previous_trans_16 |= mask;
						if (current_trans & (1<<i)) current_trans_16 |= mask;
					}

					if (p == 0) {	// if first page, there is no previous page
						buffer_mask &= ~((1 << offset) - 1);
					} else {		// else, get some pixels from previous page
						buffer_candidate |= (*previous_line_place & previous_line_mask) >> (16 - offset);
						buffer_mask &= ((previous_trans_16 >> (16-offset)) | (((1<<(16-offset))-1) << offset) );
					}

					if (p + 1 == page_total) {
						if (last_line_lines > offset) {	// if last page in loop and current line exists
							buffer_candidate |= (*current_line_place & current_line_mask) << offset;
							buffer_mask &= ((current_trans_16 << offset) | ((1<<offset)-1));
						}
						buffer_mask &= (1 << last_line_lines) - 1;
					} else {
						buffer_candidate |= (*current_line_place & current_line_mask) << offset;
						buffer_mask &= ((current_trans_16 << offset) | ((1<<offset)-1));
					}
					break;
				case SPRITE_TYPE_3:
					// TODO
					break;
				case SPRITE_TYPE_4:
					// TODO
					break;
				}

				*buffer_place &= ~buffer_mask;
				*buffer_place |= buffer_candidate & buffer_mask;
			}
		}
//	}
}
