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
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// 72 MHz / 4 = 18 MHz
	TIM_TimeBaseInitTypeDef timtb;
	TIM_TimeBaseStructInit(&timtb);
	timtb.TIM_ClockDivision = TIM_CKD_DIV4;
	timtb.TIM_CounterMode = TIM_CounterMode_Up;
	timtb.TIM_Period = 255;
	timtb.TIM_Prescaler = 0;
	timtb.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &timtb);

	TIM_OCInitTypeDef timoc;
	TIM_OCStructInit(&timoc);
	timoc.TIM_OCMode = TIM_OCMode_PWM1;
	timoc.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	timoc.TIM_OutputNState = TIM_OutputNState_Enable;
	timoc.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OC1Init(TIM1, &timoc);
	TIM_OC2Init(TIM1, &timoc);
	TIM_OC3Init(TIM1, &timoc);

	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_BDTRInitTypeDef timbdtr;
	TIM_BDTRStructInit(&timbdtr);
	timbdtr.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &timbdtr);

	TIM_Cmd(TIM1, ENABLE);

}

void led_set(uint32_t led_id, uint32_t state) {
	switch (led_id) {
	case 0:
		TIM1->CCR1 = state ? 30 : 0;
		break;
	case 1:
		TIM1->CCR2 = state ? 30 : 0;
		break;
	case 2:
		TIM1->CCR3 = state ? 30 : 0;
		break;
	}
}

void led_set_rgb(uint8_t r, uint8_t g, uint8_t b) {
	TIM1->CCR1 = r;
	TIM1->CCR2 = g;
	TIM1->CCR3 = b;
}

void led_set_hsv(uint8_t h, uint8_t s, uint8_t v) {
	unsigned char r, g, b;
    unsigned char region, remainder, p, q, t;

    if (s == 0)
    {
        r = v;
        g = v;
        b = v;
    } else {
		region = h / 43;
		remainder = (h - (region * 43)) * 6;

		p = (v * (255 - s)) >> 8;
		q = (v * (255 - ((s * remainder) >> 8))) >> 8;
		t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

		switch (region)
		{
			case 0:
				r = v; g = t; b = p;
				break;
			case 1:
				r = q; g = v; b = p;
				break;
			case 2:
				r = p; g = v; b = t;
				break;
			case 3:
				r = p; g = q; b = v;
				break;
			case 4:
				r = t; g = p; b = v;
				break;
			default:
				r = v; g = p; b = q;
				break;
		}
    }
	TIM1->CCR1 = r;
	TIM1->CCR2 = g;
	TIM1->CCR3 = b;
}
