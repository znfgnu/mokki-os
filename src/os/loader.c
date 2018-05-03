/*
 * loader.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include "os/loader.h"
#include "os/api.h"

void loader_init_fntable(program_struct_t* program) {
	// Fill function addresses
	for (int i=0; i<program->number_of_functions; ++i) {
		int id = program->functions[i].id_address;
		program->functions[i].id_address = (uint32_t) api_fn_pointers[id];
	}
	program->number_of_functions = 0;
}

program_t loader_get_launcher(program_struct_t* program) {
	// Prepare function
	return (void*)((uint32_t)program | 1);
}
