/*
 * api.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "stm32f10x.h"
#include "hw/led.h"
#include "hw/btn.h"
#include "hw/time.h"
#include "gfx/gfx.h"
#include "gfx/font.h"

const void* api_fn_pointers[] = {
		(void*) 0,			// 0x00
		(void*) led_set,	// 0x01
		(void*) btn_get,	// 0x02
		(void*) led_set_rgb,	// 0x03
		(void*) time_delay,	// 0x04
		(void*) gfx_clear,	// 0x05
		(void*) gfx_update,	// 0x06
		(void*) gfx_set_clear_color,	// 0x07
		(void*) gfx_set_pixel,	// 0x08
		(void*) font_print_string,	// 0x09
};

void api_init(void) {
}
