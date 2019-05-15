/*
 * uart.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include <hw/debug.h>
#include "stm32f10x.h"

char debug_tx_buffer[1024];
static char* debug_tx_ptr = debug_tx_buffer;
uint8_t debug_tx_flag = 0;	// 0 -> ready to send data, 1 -> sending in progress

void debug_init(void) {
	/*
	 * UART is configured on pins PA9 and PA10 of USART1 peripheral
	 */
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	// Configure TX pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Configure UART peripheral
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART5, &USART_InitStructure);
	USART_Cmd(UART5, ENABLE);

	// Configure interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void debug_tx(const char* msg) {
	while(debug_tx_flag);	// Active wait for last transmission to finish.
	while(1) {
		*debug_tx_ptr++ = *msg;
		if (*msg == '\0') break;
		msg++;
	}
	debug_tx_flag = 1;
	debug_tx_ptr = debug_tx_buffer;
	while(!(UART5->SR & USART_FLAG_TXE));
	UART5->DR = *debug_tx_ptr;
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
}


void UART5_IRQHandler() {
	if (USART_GetITStatus(UART5, USART_IT_TXE)) {
		USART_ClearITPendingBit(UART5, USART_IT_TXE);
		if (debug_tx_flag == 1) {
			UART5->DR = *++debug_tx_ptr;
			if (*debug_tx_ptr == '\0') {
				debug_tx_ptr = debug_tx_buffer;
				debug_tx_flag = 0;
				USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
			}
		}
	}
}


