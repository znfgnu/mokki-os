/*
 * time.h
 *
 *  Created on: 08.05.2018
 *      Author: konrad
 */

#ifndef OS_TIME_H_
#define OS_TIME_H_

typedef struct {
	uint32_t seconds;
	uint16_t millis;
} time_t;

extern time_t time_now;

void time_init(void);

#endif /* OS_TIME_H_ */
