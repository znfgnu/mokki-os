/*
 * loader.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef OS_LOADER_H_
#define OS_LOADER_H_

#include "program_struct.h"

void loader_init_fntable(program_struct_t* program);
program_t loader_get_launcher(program_struct_t* program);

#endif /* OS_LOADER_H_ */
