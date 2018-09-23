/*
 * time.h
 *
 *  Created on: 08.05.2018
 *      Author: konrad
 */

#ifndef OS_TIME_H_
#define OS_TIME_H_

#include "stm32f10x.h"

extern uint32_t time_now;

void time_init(void);
void time_delay(int ms);

#endif /* OS_TIME_H_ */
