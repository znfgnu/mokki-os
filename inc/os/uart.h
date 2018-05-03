/*
 * uart.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef UART_H_
#define UART_H_

#define UART_FRAME_TERMINATOR	('\n')

extern char uart_rx_buffer[];
extern uint8_t uart_rx_flag;

void uart_init(void);
void uart_tx(const char* msg);
void uart_dma_rx(void* ptr, uint32_t size);
void uart_dma_rx_cleanup(void);

#endif /* UART_H_ */
