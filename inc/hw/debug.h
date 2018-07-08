/*
 * uart.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "stm32f10x_dma.h"

#define DEBUG_FRAME_TERMINATOR	('\n')

// WARNING: not implemented yet!
// #define UART_DMA_Rx_Channel_IRQn	DMA1_Channel5_IRQn

extern char debug_rx_buffer[];
extern uint8_t debug_rx_flag;

void debug_init(void);
void debug_tx(const char* msg);
void debug_dma_rx(void* ptr, uint32_t size);
void debug_dma_rx_cleanup(void);

#endif /* DEBUG_H_ */
