/*
 * led.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void led_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

void led_set(uint32_t led_id, uint32_t state) {
	// Ignores led_id for now
	switch (led_id) {
	case 0:
		GPIOB->BSRR = !state ? GPIO_Pin_13 : (GPIO_Pin_13 << 16);
		break;
	case 1:
		GPIOB->BSRR = !state ? GPIO_Pin_14 : (GPIO_Pin_14 << 16);
		break;
	case 2:
		GPIOB->BSRR = !state ? GPIO_Pin_15 : (GPIO_Pin_15 << 16);
		break;
	}
}
