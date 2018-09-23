/*
 * time.c
 *
 *  Created on: 08.05.2018
 *      Author: konrad
 */

#include "stm32f10x.h"
#include "hw/time.h"

uint32_t time_now;

void time_init(void) {
	SysTick_Config(SystemCoreClock/1000);
}

void time_delay(int ms) {
	ms += time_now;
	while(time_now < ms);
}

void SysTick_Handler(void) {
	++time_now;
}
