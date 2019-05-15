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

void debug_init(void);
void debug_tx(const char* msg);

#endif /* DEBUG_H_ */
