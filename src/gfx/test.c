/*
 * test.c
 *
 *  Created on: 13.05.2018
 *      Author: konrad
 */

#include "hw/oled.h"

void gfx_test(void) {
	int cntr = 0;
	int fps = 0;
	char sfps[4] = {'\0', '\0', '\0', '\0'};
	while(1) {
		oled_clear_buffer();

		sfps[0] = '0' + (fps/100);
		sfps[1] = '0' + ((fps/10) % 10);
		sfps[2] = '0' + (fps % 10);

		font_print_string(sfps, 0, 0, oled_buffer);

		oled_update_screen();
	}
}
