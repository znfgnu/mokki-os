/*
 * api.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "stm32f10x.h"
#include "hw/led.h"
#include "hw/btn.h"

const void* api_fn_pointers[] = {
		(void*) 0,			// 0x00
		(void*) led_set,	// 0x01
		(void*) btn_get,	// 0x02
};

void api_init(void) {
}
