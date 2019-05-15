/*
 * bt.c
 *
 *  Created on: 13.05.2019
 *      Author: konrad
 */

#include "stm32f10x.h"
#include "hw/bt.h"
#include "os/supervisor.h"

char bt_rx_buffer[1024];
static char* bt_rx_ptr = bt_rx_buffer;
uint8_t bt_rx_flag = 0;	// 0 -> filling in progress, 1 -> reading in progress

char bt_tx_buffer[1024];
static char* bt_tx_ptr = bt_tx_buffer;
int bt_tx_cntr = 0;
uint8_t bt_tx_flag = 0;	// 0 -> ready to send data, 1 -> sending in progress

enum {
	BT_HEADER,
	BT_ATRESP,
	BT_PROTOC,
} bt_rx_state = BT_HEADER;
int bt_rx_cntr = 0;

supervisorRequest_t bt_lastRequest = { .header = 0x00 };

void UART4_IRQHandler() {
	if (USART_GetITStatus(UART_PERIPH, USART_IT_RXNE)) {
		if (bt_rx_flag == 0) {
			*bt_rx_ptr = UART_PERIPH->DR;
			switch (bt_rx_state) {
			case BT_HEADER:
				if (*bt_rx_ptr == '+') {
					bt_rx_state = BT_ATRESP;
				} else if (*bt_rx_ptr < REQ_TOTAL_HEADERS && *bt_rx_ptr != '\0') {
					bt_rx_state = BT_PROTOC;
					bt_rx_cntr = supervisor_request_payloadSize[*bt_rx_ptr];
				} else {
					for(;;);
				}
				bt_rx_ptr++;
				break;
			case BT_ATRESP:
				if (*bt_rx_ptr == BT_FRAME_TERMINATOR) {
					bt_rx_ptr = bt_rx_buffer;
					bt_rx_flag = 1;
					bt_rx_state = BT_HEADER;
				} else {
					bt_rx_ptr++;
				}
				break;
			case BT_PROTOC:
				bt_rx_cntr--;
				bt_rx_ptr++;
				if (!bt_rx_cntr) {
					// Frame full
					if (!bt_lastRequest.header) {
						bt_lastRequest.header = bt_rx_buffer[0];
						for (int i=1; i < bt_rx_ptr-bt_rx_buffer; ++i) {
							bt_lastRequest.payload.raw[i-1] = bt_rx_buffer[i];
						}
						SUPERVISOR_INVOKE();
					}
					bt_rx_state = BT_HEADER;
					bt_rx_ptr = bt_rx_buffer;
				}
				break;
			}
		} else {
			USART_ClearITPendingBit(UART_PERIPH, USART_IT_RXNE);	// reading data register clears flag
		}
//		if (bt_rx_flag == 0) {
//			*bt_rx_ptr = UART_PERIPH->DR;
//			if (*bt_rx_ptr == BT_FRAME_TERMINATOR) {
//				// go to next buffer
//				bt_rx_ptr = bt_rx_buffer;
//				bt_rx_flag = 1;
//				SUPERVISOR_INVOKE();
//			} else {
//				bt_rx_ptr++;
//			}
//		} else {
//			// error, no space for more bytes
//			USART_ClearITPendingBit(UART_PERIPH, USART_IT_RXNE);	// reading data register clears flag
//		}
	}
	if (USART_GetITStatus(UART_PERIPH, USART_IT_TXE)) {
		USART_ClearITPendingBit(UART_PERIPH, USART_IT_TXE);
		if (bt_tx_flag == 1) {
			UART_PERIPH->DR = *++bt_tx_ptr;
			bt_tx_cntr--;
			if (!bt_tx_cntr) {
				bt_tx_ptr = bt_tx_buffer;
				bt_tx_flag = 0;
				USART_ITConfig(UART_PERIPH, USART_IT_TXE, DISABLE);
			}
		}
	}
}

void bt_tx(const char* msg) {
	while(bt_tx_flag);	// Active wait for last transmission to finish.
	bt_tx_cntr = 0;
	while(1) {
		bt_tx_cntr++;
		*bt_tx_ptr++ = *msg;
		if (*msg == '\0') break;
		msg++;
	}
	bt_tx_flag = 1;
	bt_tx_ptr = bt_tx_buffer;
	while(!(UART_PERIPH->SR & USART_FLAG_TXE));
	UART_PERIPH->DR = *bt_tx_ptr;
	USART_ITConfig(UART_PERIPH, USART_IT_TXE, ENABLE);
}

void bt_tx_res(supervisorResponse_t* resp) {
	while(bt_tx_flag);	// Active wait for last transmission to finish.
	*bt_tx_ptr++ = resp->header;
	for (int i=0; i<supervisor_response_payloadSize[resp->header]; ++i) {
		*bt_tx_ptr++ = resp->payload.raw[i];
	}
	bt_tx_flag = 1;
	bt_tx_ptr = bt_tx_buffer;
	bt_tx_cntr = supervisor_response_payloadSize[resp->header] + 1;
	while(!(UART_PERIPH->SR & USART_FLAG_TXE));
	UART_PERIPH->DR = *bt_tx_ptr;
	USART_ITConfig(UART_PERIPH, USART_IT_TXE, ENABLE);
}

void bt_rx(char* buf) {
	while(!bt_rx_flag);	// Active wait for last transmission to finish.
	char* ptr = bt_rx_buffer;
	while(1) {
//		*bt_tx_ptr++ = *msg;
		*buf = *ptr++;
		if (*buf == BT_FRAME_TERMINATOR) break;
		buf++;
	}
	bt_rx_flag = 0;
	bt_rx_ptr = bt_rx_buffer;
}

void bt_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	GPIO_InitTypeDef gpio;
	USART_InitTypeDef uart;

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = PWRC_PIN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(PWRC_PORT, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = STAT_PIN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(STAT_PORT, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = UART_RX_PIN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(UART_PORT, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = UART_TX_PIN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(UART_PORT, &gpio);

	uart.USART_BaudRate = 115200;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART_PERIPH, &uart);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = UART4_IRQn;					// TODO: macro!
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0x1;
	nvic.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_Init(&nvic);

	USART_ITConfig(UART_PERIPH, USART_IT_RXNE, ENABLE);

	USART_Cmd(UART_PERIPH, ENABLE);

	GPIO_WriteBit(PWRC_PORT, PWRC_PIN, RESET);
	for(int i=0; i<1000000; ++i);
	GPIO_WriteBit(PWRC_PORT, PWRC_PIN, SET);
	for(int i=0; i<1000000; ++i);
	GPIO_WriteBit(PWRC_PORT, PWRC_PIN, RESET);
	for(int i=0; i<1000000; ++i);
}

int bt_sendmsg(const char* buffer) {
	bt_tx(buffer);
	return 0;
	register int counter = 0;
	while(*buffer){
		while(!USART_GetFlagStatus(UART_PERIPH, USART_FLAG_TXE));
		UART_PERIPH->DR = *buffer;
		++counter;
		++buffer;
	}
	return counter;
}

//int bt_senddata(const char* buffer, int n) {
//	register int counter = 0;
//	while(counter != n) {
//		while(!USART_GetFlagStatus(UART_PERIPH, USART_FLAG_TXE));
//		UART_PERIPH->DR = *buffer;
//		++counter;
//		++buffer;
//	}
//	return counter;
//}

int bt_recvmsg(char* buffer) {
	bt_rx(buffer);
	return 0;
	register int counter = 0;
	while(1) {
		while(!USART_GetFlagStatus(UART_PERIPH, USART_FLAG_RXNE));
		*buffer = UART_PERIPH->DR;
		++counter;
		if (*buffer == '\n') break;
		++buffer;
	}
	return counter;
}

void bt_mode(btMode_t mode) {
	switch(mode) {
	case BTMODE_CMD:
		GPIO_WriteBit(PWRC_PORT, PWRC_PIN, RESET);
//		PWRC_PORT->BSRRH = PWRC_PIN;	// PWRC LOW - AT command mode
		break;
	case BTMODE_DATA:
		GPIO_WriteBit(PWRC_PORT, PWRC_PIN, SET);
//		PWRC_PORT->BSRRL = PWRC_PIN;	// PWRC HIGH - data mode
		break;
	}
}

void bt_test(void) {
	char response[100];
	bt_init();
	bt_sendmsg("AT+RST");
	bt_recvmsg(response);
	for(int i=0; i<5000000; ++i);
	bt_sendmsg("AT+NAMEMokki-dev");
	bt_recvmsg(response);
	bt_mode(BTMODE_DATA);

//	for(;;) {
//		int n = bt_recvmsg(response);
//		response[n-1] = '!';
//		bt_senddata(response, n);
//	}

	for(int k=0; ; ++k) {
		char buf[20];
		for(int i=0; i<10000000; ++i);
		sprintf(buf, "Licze do %d", k);
		bt_sendmsg(buf);
	}
}
