/*
 * supervisor.h
 *
 *  Created on: 14.05.2019
 *      Author: konrad
 */

#ifndef OS_SUPERVISOR_H_
#define OS_SUPERVISOR_H_

#include "stm32f10x.h"
#define SUPERVISOR_INVOKE() do { SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; } while(0);

typedef enum {
	REQ_NONE,
	REQ_GET,
	REQ_SET,
	REQ_DATA,
	REQ_PRO,
	REQ_CMD,
	REQ_TOTAL_HEADERS,
} supervisorRequestHeader_t;

extern const int supervisor_request_payloadSize[REQ_TOTAL_HEADERS];

typedef struct __attribute__((packed)) {
	uint8_t header;
	union __attribute__((packed)) {
		uint8_t raw[20];
		struct __attribute__((packed)){
			uint16_t prop_id;
			uint32_t index;
		} get;
		struct __attribute__((packed)){
			uint16_t prop_id;
			uint32_t data;
		} set;
		struct __attribute__((packed)){
			uint16_t packet_no;
			uint8_t data[16];
		} data;
		struct __attribute__((packed)){
			uint32_t size;
		} pro;
		struct __attribute__((packed)){
			uint16_t cmd_no;
		} cmd;
	} payload;
} supervisorRequest_t;

// ----- Responses

typedef enum {
	RESP_NONE,
	RESP_OK,
	RESP_OK_DATA,
	RESP_FAIL,
	RESP_FAIL_DATA,
	RESP_TOTAL_HEADERS,
} supervisorResponseHeader_t;

extern const int supervisor_response_payloadSize[RESP_TOTAL_HEADERS];

typedef struct __attribute__((packed)) {
	uint8_t header;
	union __attribute__((packed)) {
		uint8_t raw[16];
		uint32_t u32;
		uint16_t u16;
		uint8_t u8;
	} payload;
} supervisorResponse_t;

void supervisor_init(void);

#endif /* OS_SUPERVISOR_H_ */
