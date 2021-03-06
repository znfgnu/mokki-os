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
		GPIOC,
		GPIOB,
		GPIOB,
		GPIOA,
};

static const uint16_t btn_pin[8] = {
		GPIO_Pin_0,
		GPIO_Pin_13,
		GPIO_Pin_0,
		GPIO_Pin_1,
		GPIO_Pin_2,
		GPIO_Pin_12,
		GPIO_Pin_2,
		GPIO_Pin_8,
};

void btn_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_13;
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

uint8_t btn_get_all(void) {
	uint8_t res = 0x00;
	for (int i=0; i<8; ++i) {
		res <<= 1;
		res |= btn_gpio[7-i]->IDR & btn_pin[7-i] ? 0 : 1;
	}
	return res;
}

