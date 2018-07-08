/*
 * shell.c
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#include <hw/debug.h>
#include "os/loader.h"
#include "os/program.h"

static const char* string_ok = "OK.";
static const char* string_ready = "Ready.";
static const char* string_error = "Error.";

static int shell_get_arg_int(const char** msg) {
	const char* ptr = *msg;
	int result = 0;
	while(*ptr < '0' || *ptr > '9') {
		if (*ptr == '\0') break;
		++ptr;
	}
	while(*ptr >= '0' && *ptr <= '9') {
		if (*ptr == '\0') break;
		result *= 10;
		result += *ptr++ - '0';
	}
	*msg = ptr;
	return result;
}

void shell_init(void) {

}

void shell_parse(const char* msg) {
	char cmd = *msg++;

	if (cmd == 'i') {
		// init function table
		loader_init_fntable((program_struct_t*) program_data);
		debug_tx(string_ok);
	}
	else if (cmd == 'r') {
		// run program
		int arg = shell_get_arg_int(&msg);
		program_t program = loader_get_launcher((program_struct_t*) program_data);
		int ret = program(arg, 0);

		// Dirty parse
		char response[] = "Return code: 000";
		response[13] = '0' + ret/100;
		response[14] = '0' + (ret/10)%10;
		response[15] = '0' + ret%10;
		debug_tx(response);
		debug_tx(string_ok);
	}
	else if (cmd == 'l') {
		int arg = shell_get_arg_int(&msg);
		if (arg > PROGRAM_MAX_SIZE) {
			debug_tx(string_error);
			return;
		}
		debug_dma_rx(program_data, arg);
		debug_tx(string_ready);
		// Wait for transfer complete
		while(!DMA_GetFlagStatus(DMA1_FLAG_TC5));
		debug_dma_rx_cleanup();
		debug_tx(string_ok);
	}
}
