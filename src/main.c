/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include <hw/debug.h>
#include "stm32f10x.h"

#include "hw/btn.h"
#include "hw/debug.h"
#include "hw/dma.h"
#include "hw/led.h"
#include "hw/oled.h"
#include "hw/time.h"

#include "os/api.h"
#include "os/loader.h"
#include "os/program_struct.h"
#include "os/program.h"
#include "os/shell.h"

#include "gfx/gfx.h"

uint8_t pattern[3][3] = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
};

//uint8_t pattern2[2][3] = {
//		{0b10010010, 0b01001001, 0b00100100},
//		{0b01101101, 0b10110110, 0b11011011},
//};

uint8_t pattern2[2][3] = {
		{0b00100100, 0b01001001, 0b10010010},
		{0b11011011, 0b10110110, 0b01101101},
};


int main(void)
{
	// Hardware init
	btn_init();
	debug_init();
	dma_init();
	led_init();
	oled_init();
	time_init();

	// System init
	api_init();
	shell_init();

	// Post-init init
	oled_initialize_screen();
	oled_start_screen_transmission();

	for(int cntr=0; cntr<3; ++cntr) {
		for (int l=0; l<8; ++l) {
			for (int c=0; c<128; ++c) {
				if (((l/2+c/16)&3) == 0) {	// black
					oled_triple_buffer[cntr][l][c] = 0x00;
				} else if (((l/2+c/16)&3) == 1) {	// gray dark
					uint8_t res = pattern2[0][(cntr+c-l+12)%3];
					oled_triple_buffer[cntr][l][c] = res;//(((c+cntr)%3) == 0) ? 0xff : 0x00;
				} else if (((l/2+c/16)&3) == 2) {	// gray light
					uint8_t res = pattern2[1][(cntr+c-l+12)%3];
					oled_triple_buffer[cntr][l][c] = res;//(((c+cntr)%3) != 0) ? 0xff : 0x00;
				} else {	// white
					oled_triple_buffer[cntr][l][c] = 0xff;//0xff;
				}
			}
		}
	}

	int cntr = 0;

	int xx[4] = {20, 30, 87, 103};
	int yy[4] = {60, 20, 55, 7};
	int dx[4] = {-2, -1, 2, 1};
	int dy[4] = {-2, 2, 1, -1};

	while(1) {
		led_set(0, (cntr / 30) & 1);

		gfx_clear_buffer();


		if (btn_get(1)) {
			for (int l = 0; l<64; ++l) {
				for (int c = 0; c<128; ++c) {
					gfx_set_pixel(l, c, COLOR_WHITE);
				}
			}
		} else if (btn_get(2)) {
			for (int l = 0; l<64; ++l) {
				for (int c = 0; c<128; ++c) {
					gfx_set_pixel(l, c, COLOR_LIGHTGRAY);
				}
			}
		} else if (btn_get(3)) {
			for (int l = 0; l<64; ++l) {
				for (int c = 0; c<128; ++c) {
					gfx_set_pixel(l, c, COLOR_DARKGRAY);
				}
			}
		} else if (btn_get(4)) {
			for (int l = 0; l<64; ++l) {
				for (int c = 0; c<128; ++c) {
					gfx_set_pixel(l, c, COLOR_BLACK);
				}
			}
		} else {
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
				gfx_set_pixel(yy[i]-1, xx[i]-1, i);
				gfx_set_pixel(yy[i]-1, xx[i], i);
				gfx_set_pixel(yy[i]-1, xx[i]+1, i);
				gfx_set_pixel(yy[i], xx[i]-1, i);
				gfx_set_pixel(yy[i], xx[i], i);
				gfx_set_pixel(yy[i], xx[i]+1, i);
				gfx_set_pixel(yy[i]+1, xx[i]-1, i);
				gfx_set_pixel(yy[i]+1, xx[i], i);
				gfx_set_pixel(yy[i]+1, xx[i]+1, i);
			}





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
		}




//		uint8_t btn_state = 0;
//		for (int i=0; i<8; ++i) {
//			btn_state |= btn_get(i) << i;
//		}
//
//		for (int i=0; i<8; ++i) {
//			oled_triple_buffer[0][0][i] = btn_state;
//		}

		++cntr;
		gfx_update();
	}

	for(;;);
}
