/*
 * btn.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef HW_BTN_H_
#define HW_BTN_H_

#include "stm32f10x.h"

#define BTN_START  (1<<0)
#define BTN_UP     (1<<1)
#define BTN_LEFT   (1<<2)
#define BTN_RIGHT  (1<<3)
#define BTN_DOWN   (1<<4)
#define BTN_A      (1<<5)
#define BTN_B      (1<<6)
#define BTN_R1     (1<<7)

void btn_init(void);
uint32_t btn_get(uint32_t btn_id);
uint8_t btn_get_all(void);

#endif /* HW_BTN_H_ */
