/*
 * program_struct.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef PROGRAM_STRUCT_H_
#define PROGRAM_STRUCT_H_

#include "stm32f10x.h"

typedef struct {
	uint16_t push;
	uint16_t load_register;
	uint16_t jump;
	uint16_t pop;
	uint32_t id_address;
} program_function_t;

typedef struct {
	uint16_t jump_to_main;
	uint16_t number_of_functions;
	uint32_t alignment[3];
	program_function_t functions[];
} program_struct_t;

typedef int (*program_t)(int, void*);

#endif /* PROGRAM_STRUCT_H_ */
