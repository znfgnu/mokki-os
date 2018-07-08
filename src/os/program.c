#include "stm32f10x.h"
#include "os/program.h"

uint16_t program_data[PROGRAM_MAX_SIZE] = {
		// Program code
		0xe008,
		0x2,
		0x4f00,
		0x4738,
		0x1,
		0x0,
		0x4f00,
		0x4738,
		0x2,
		0x0,
		0xfb00,
		0xf000,
		0x4770,
		0xbf00,
};
// Total halfwords (code/offset/total): 14/0/14
// Bytes used (code/offset/total): 28/0/28
