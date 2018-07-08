/*
 * dma.c
 *
 *  Created on: 06.05.2018
 *      Author: konrad
 */

#include <hw/debug.h>
#include "stm32f10x_rcc.h"

#include "hw/oled.h"

void dma_init(void) {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	// Enable interrupts
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

//	NVIC_InitStructure.NVIC_IRQChannel = UART_DMA_Rx_Channel_IRQn;//DMA1_Channel6_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = OLED_DMA_Channel_IRQn;//DMA1_Channel6_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

