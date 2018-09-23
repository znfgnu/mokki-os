/*
 * raminfo.c
 *
 *  Created on: 23.09.2018
 *      Author: konrad
 */

#include "stm32f10x.h"

extern char _sdata;
extern char _ebss;
extern char _estack;

uint32_t raminfo_total(void) {
	return &_estack - &_sdata;
}
uint32_t raminfo_static(void) {
	return &_ebss - &_sdata;
}
uint32_t raminfo_stack(void) {
	return &_estack - (char*)__builtin_frame_address(0);
}
uint32_t raminfo_free(void) {
	return (char*)__builtin_frame_address(0) - &_ebss;
}

