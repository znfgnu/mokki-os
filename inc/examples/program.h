/*
 * program.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "stm32f10x.h"

#define PROGRAM_MAX_SIZE	512	// == 1024 bytes
extern uint16_t program_data[PROGRAM_MAX_SIZE];

#endif /* PROGRAM_H_ */
