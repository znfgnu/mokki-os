/*
 * test.c
 *
 *  Created on: 18.08.2018
 *      Author: konrad
 */

#include "hw/led.h"
#include "gfx/gfx.h"

extern const uint16_t dragon[8][64];
extern const uint16_t dragon2[8][64];
extern const uint16_t concept[8][128];

void run_test(void) {
	int cntr = 0;

	int xx[4] = {20, 30, 87, 103};
	int yy[4] = {60, 20, 55, 7};
	int dx[4] = {-2, -1, 2, 1};
	int dy[4] = {-2, 2, 1, -1};

	int dragonx = 0;
	int dragondx = 1;

	gfx_set_clear_color(COLOR_DARKGRAY);
	while(1) {
		led_set_hsv(cntr & 0xFF, 0xFF, 0x7F);

		if (btn_get(1)) {
			gfx_set_clear_color(COLOR_WHITE);
		} else if (btn_get(2)) {
			gfx_set_clear_color(COLOR_LIGHTGRAY);
		} else if (btn_get(3)) {
			gfx_set_clear_color(COLOR_DARKGRAY);
		} else if (btn_get(4)) {
			gfx_set_clear_color(COLOR_BLACK);
		}

		gfx_clear();

		int d1 = btn_get(5);
		int d2 = btn_get(6);

		if (d1 || d2) {
			// Blit moving dragon
			if (d1) {
				for (int p = 0; p<8; ++p) {
					for (int c = 0; c<64; ++c) {
						gfx_buffer[p][c+dragonx] = dragon[p][c];
					}
				}
			} else if (d2) {
				for (int p = 0; p<8; ++p) {
					for (int c = 0; c<64; ++c) {
						gfx_buffer[p][c+dragonx] = dragon2[p][c];
					}
				}
			}

			if (btn_get(7)) {
				dragonx += dragondx;
				if (dragonx >= 64 || dragonx < 0) {
					dragondx = -dragondx;
					dragonx += 2 * dragondx;
				}
			}
		} else {
			if (btn_get(0)) {
				for (int i=0; i<4; ++i) {
					xx[i] += dx[i];
					if (xx[i] >= OLED_WIDTH-1 || xx[i] < 1) {
						dx[i] = -dx[i];
						xx[i] += dx[i] * 2;
					}
					yy[i] += dy[i];
					if (yy[i] >= OLED_HEIGHT-1 || yy[i] < 1) {
						dy[i] = -dy[i];
						yy[i] += dy[i] * 2;
					}
					gfx_set_pixel(xx[i]-1, yy[i]-1, i);
					gfx_set_pixel(xx[i]-1, yy[i], i);
					gfx_set_pixel(xx[i]-1, yy[i]+1, i);
					gfx_set_pixel(xx[i], yy[i]-1, i);
					gfx_set_pixel(xx[i], yy[i], i);
					gfx_set_pixel(xx[i], yy[i]+1, i);
					gfx_set_pixel(xx[i]+1, yy[i]-1, i);
					gfx_set_pixel(xx[i]+1, yy[i], i);
					gfx_set_pixel(xx[i]+1, yy[i]+1, i);
				}

				// Lines
				for (int i=0; i<64; ++i) {
					gfx_set_pixel(i, i, COLOR_WHITE);
					gfx_set_pixel(i, i+1, COLOR_WHITE);
					gfx_set_pixel(i, i+2, COLOR_BLACK);
					gfx_set_pixel(i, i+3, COLOR_BLACK);
					gfx_set_pixel(i, i+4, COLOR_WHITE);
					gfx_set_pixel(i, i+5, COLOR_WHITE);
					gfx_set_pixel(i, i+6, COLOR_DARKGRAY);
					gfx_set_pixel(i, i+7, COLOR_DARKGRAY);
					gfx_set_pixel(i, i+8, COLOR_WHITE);
					gfx_set_pixel(i, i+9, COLOR_WHITE);
					gfx_set_pixel(i, i+10, COLOR_LIGHTGRAY);
					gfx_set_pixel(i, i+11, COLOR_LIGHTGRAY);
					gfx_set_pixel(i, i+12, COLOR_WHITE);
					gfx_set_pixel(i, i+13, COLOR_WHITE);
				}
			} else {
				for (int p = 0; p<8; ++p) {
					for (int c = 0; c<128; ++c) {
						gfx_buffer[p][c] = concept[p][c];
					}
				}
			}

			if (btn_get(7)) {
				font_print_string(10, 28, "Hello, World!", COLOR_WHITE);
			}
		}

		++cntr;
		gfx_update();
	}
}
