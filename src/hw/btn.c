/*
 * btn.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

static const GPIO_TypeDef* btn_gpio[8] = {
		GPIOA,
		GPIOC,
		GPIOC,
		GPIOC,
		GPIOA,
		GPIOB,
		GPIOB,
		GPIOA,
};

static const uint16_t btn_pin[8] = {
		GPIO_Pin_4,
		GPIO_Pin_13,
		GPIO_Pin_14,
		GPIO_Pin_15,
		GPIO_Pin_0,
		GPIO_Pin_13,
		GPIO_Pin_14,
		GPIO_Pin_15,
};

void btn_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

uint32_t btn_get(uint32_t btn_id) {
	if (btn_id >= 8) {
		return -1;
	}

	int res = btn_gpio[btn_id]->IDR & btn_pin[btn_id];
	// Non-zero result -> button released
	return res ? 0 : 1;
}

