/*
 * btn.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

void btn_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

uint32_t btn_get(uint32_t btn_id) {
	if (btn_id >= 4) {
		return -1;
	}

	int res = GPIOB->IDR & (GPIO_Pin_12 << btn_id);
	// Non-zero result -> button released
	return res ? 0 : 1;
}

