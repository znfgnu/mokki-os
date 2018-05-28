/*
 * time.c
 *
 *  Created on: 08.05.2018
 *      Author: konrad
 */

#include "stm32f10x.h"
#include "hw/time.h"

time_t time_now;

void time_init(void) {
	SysTick_Config(SystemCoreClock/1000);
}

void SysTick_Handler(void) {
	++time_now.millis;
	if (time_now.millis == 1000) {
		++time_now.seconds;
		time_now.millis = 0;
	}
}
