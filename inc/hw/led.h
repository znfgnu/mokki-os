/*
 * led.h
 *
 *  Created on: 24.04.2018
 *      Author: konrad
 */

#ifndef HW_LED_H_
#define HW_LED_H_

#include "stm32f10x.h"

void led_init(void);
void led_set(uint32_t led_id, uint32_t state);
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b);
void led_set_hsv(uint8_t h, uint8_t s, uint8_t v);

#endif /* HW_LED_H_ */
