/*
 * supervisor.c
 *
 *  Created on: 14.05.2019
 *      Author: konrad
 */

#include "stm32f10x.h"
#include "gui/gui.h"
#include "hw/bt.h"
#include "os/supervisor.h"
#include "hw/oled.h"

const int supervisor_request_payloadSize[REQ_TOTAL_HEADERS] = {-1, 6, 6, 18, 4, 2};
const int supervisor_response_payloadSize[RESP_TOTAL_HEADERS] = {-1, 0, 16, 0, 16};

extern char _ebss;
uint8_t rebootRequested = 0;

void supervisor_init(void) {
	// init pendsv
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = PendSV_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x07;
	nvic.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&nvic);
}

void PendSV_Handler(void) {
	// 1. Handle request from bt_lastRequest
	// 2. Set its header to REQ_NONE to indicate that handling is over
	// 3. Send supervisorResponse
	char buf[100];
	sprintf(buf, "type %d\n", bt_lastRequest.header);

	switch(bt_lastRequest.header) {
	case REQ_DATA:
		for (int i=0; i<16; ++i) {
		  ((char*)(&_ebss))[bt_lastRequest.payload.data.packet_no*16+i] = bt_lastRequest.payload.data.data[i];
		}
		break;
	case REQ_CMD:
		switch(bt_lastRequest.payload.cmd.cmd_no) {
		case 0:
			rebootRequested = 1;
			while(oled_transfer_in_progress);
			NVIC_SystemReset();
			break;
		case 1:
			externalRun = 1;
			break;
		}
	}

	bt_lastRequest.header = REQ_NONE;

	debug_tx(buf);
	supervisorResponse_t resp;
	resp.header = RESP_OK;
	for (int i=0; i<16; ++i) resp.payload.raw[i] = 0x00;
//	resp.payload.u32 = pendcntr;
	bt_tx_res(&resp);
}
