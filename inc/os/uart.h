/*
 * uart.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f10x_dma.h"

#define UART_FRAME_TERMINATOR	('\n')

// WARNING: not implemented yet!
// #define UART_DMA_Rx_Channel_IRQn	DMA1_Channel5_IRQn

extern char uart_rx_buffer[];
extern uint8_t uart_rx_flag;

void uart_init(void);
void uart_tx(const char* msg);
void uart_dma_rx(void* ptr, uint32_t size);
void uart_dma_rx_cleanup(void);

#endif /* UART_H_ */
