/*
 * led.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void led_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void led_set(uint32_t led_id, uint32_t state) {
	// Ignores led_id for now
	GPIOA->BSRR = state ? GPIO_Pin_8 : (GPIO_Pin_8 << 16);
}
