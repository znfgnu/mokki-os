/*
 * clock.c
 *
 *  Created on: 10.07.2018
 *      Author: konrad
 */

#include "stm32f10x.h"

void clock_init(void) {
	RCC_DeInit();

//	RCC_HSICmd(ENABLE);
//	while(!RCC_GetFlagStatus(RCC_FLAG_HSIRDY));
//	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_10);
//
	RCC_HSEConfig(RCC_HSE_ON);
	while(!RCC_GetFlagStatus(RCC_FLAG_HSERDY));
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);

	RCC_PLLCmd(ENABLE);
	while (!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	SystemCoreClockUpdate();
}
