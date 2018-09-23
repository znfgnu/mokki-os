/*
 * raminfo.h
 *
 *  Created on: 23.09.2018
 *      Author: konrad
 */

#ifndef OS_RAMINFO_H_
#define OS_RAMINFO_H_

#include "stm32f10x.h"

uint32_t raminfo_total(void);
uint32_t raminfo_static(void);
uint32_t raminfo_stack(void);
uint32_t raminfo_free(void);

#endif /* OS_RAMINFO_H_ */
