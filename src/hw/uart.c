/*
 * uart.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "stm32f10x.h"

#include "hw/uart.h"

char uart_rx_buffer[1024];
static char* uart_rx_ptr = uart_rx_buffer;
uint8_t uart_rx_flag = 0;	// 0 -> filling in progress, 1 -> reading in progress

char uart_tx_buffer[1024];
static char* uart_tx_ptr = uart_tx_buffer;
uint8_t uart_tx_flag = 0;	// 0 -> ready to send data, 1 -> sending in progress

void uart_init(void) {
	/*
	 * UART is configured on pins PA9 and PA10 of USART1 peripheral
	 */
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

	// Configure TX pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure RX pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure UART peripheral
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

	// Configure interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// TODO Configure DMA structures
}

void uart_tx(const char* msg) {
	while(uart_tx_flag);	// Active wait for last transmission to finish.
	while(1) {
		*uart_tx_ptr++ = *msg;
		if (*msg == '\0') break;
		msg++;
	}
	uart_tx_flag = 1;
	uart_tx_ptr = uart_tx_buffer;
	while(!(USART1->SR & USART_FLAG_TXE));
	USART1->DR = *uart_tx_ptr;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

// --------------------------- DMA Handling

#define USARTy_Rx_DMA_Channel    DMA1_Channel5
#define USARTy_Rx_DMA_FLAG       DMA1_FLAG_TC5

void uart_dma_rx(void* ptr, uint32_t size) {
	// Disable DMA channel before clearing interrupt flags
	USARTy_Rx_DMA_Channel->CCR = 0;	// Control register
	// Clear interrupts
	DMA1->IFCR |= ((uint32_t)(DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5));	// Interrupt flag clear register
	// Init DMA
	USARTy_Rx_DMA_Channel->CCR = \
			DMA_DIR_PeripheralSRC | \
			DMA_Mode_Normal | \
			DMA_PeripheralInc_Disable | \
			DMA_MemoryInc_Enable | \
			DMA_PeripheralDataSize_Byte | \
			DMA_MemoryDataSize_Byte | \
			DMA_Priority_VeryHigh | \
			DMA_M2M_Disable;
	USARTy_Rx_DMA_Channel->CMAR = (uint32_t) ptr;	// Memory address
	USARTy_Rx_DMA_Channel->CPAR = (uint32_t) &(USART1->DR);	// Peripheral address
	USARTy_Rx_DMA_Channel->CNDTR = size;		// Remaining bytes

	// Turn on DMA request
	USART1->CR3 |= USART_DMAReq_Rx;
	// Turn on DMA
	USARTy_Rx_DMA_Channel->CCR |= DMA_CCR1_EN;
}

void uart_dma_rx_cleanup(void) {
	// USART_DMACmd(USART1, USART_DMAReq_Rx, DISABLE);
	USART1->CR3 &= (~USART_DMAReq_Rx);
	// DMA_Cmd(USARTy_Rx_DMA_Channel, DISABLE);
	USARTy_Rx_DMA_Channel->CCR = 0;
}

// --------------------------- Interrupt handlers

void USART1_IRQHandler() {
	if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
		if (uart_rx_flag == 0) {
			*uart_rx_ptr = USART1->DR;
			if (*uart_rx_ptr == UART_FRAME_TERMINATOR) {
				// go to next buffer
				uart_rx_ptr = uart_rx_buffer;
				uart_rx_flag = 1;
			} else {
				uart_rx_ptr++;
			}
		} else {
			// error, no space for more bytes
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);	// reading data register clears flag
		}
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE)) {
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
		if (uart_tx_flag == 1) {
			USART1->DR = *++uart_tx_ptr;
			if (*uart_tx_ptr == '\0') {
				uart_tx_ptr = uart_tx_buffer;
				uart_tx_flag = 0;
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			}
		}
	}
}


