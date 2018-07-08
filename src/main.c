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

	uint32_t cntr = 0;

	while(1) {
		led_set(0, (cntr / 70) & 1);

		for (int l=0; l<8; ++l) {
			for (int c=0; c<128; ++c) {
				if (((l/2+c/16)&3) == 0) {	// black
					oled_buffer_tx[l][c] = 0x00;
				} else if (((l/2+c/16)&3) == 1) {	// gray dark
					uint8_t res = pattern2[0][(cntr+c+l)%3];
					oled_buffer_tx[l][c] = res;//(((c+cntr)%3) == 0) ? 0xff : 0x00;
				} else if (((l/2+c/16)&3) == 2) {	// gray light
					uint8_t res = pattern2[1][(cntr+c+l)%3];
					oled_buffer_tx[l][c] = res;//(((c+cntr)%3) != 0) ? 0xff : 0x00;
				} else {	// white
					oled_buffer_tx[l][c] = 0xff;//0xff;
				}
			}
		}

		uint8_t btn_state = 0;
		for (int i=0; i<8; ++i) {
			btn_state |= btn_get(i) << i;
		}

		for (int i=0; i<8; ++i) {
			oled_buffer_tx[0][i] = btn_state;
		}

		++cntr;
		oled_update_screen();
	}

	for(;;);
}
