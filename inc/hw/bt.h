/*
 * bt.h
 *
 *  Created on: 13.05.2019
 *      Author: konrad
 */

#ifndef HW_BT_H_
#define HW_BT_H_

#include "os/supervisor.h"

#define PWRC_PORT    GPIOD
#define PWRC_PIN     GPIO_Pin_2
#define STAT_PORT    GPIOA
#define STAT_PIN     GPIO_Pin_15
#define UART_PORT    GPIOC
#define UART_RX_PIN  GPIO_Pin_11
#define UART_RX_PS   GPIO_PinSource10
#define UART_TX_PIN  GPIO_Pin_10
#define UART_TX_PS   GPIO_PinSource11

#define UART_PERIPH  UART4
#define UART_AF      GPIO_AF_UART4

#define BT_FRAME_TERMINATOR	('\n')

typedef enum {
	BTMODE_CMD,
	BTMODE_DATA,
} btMode_t;

extern supervisorRequest_t bt_lastRequest;

void bt_init(void);
int bt_recvmsg(char* buffer);
int bt_sendmsg(const char* buffer);
int bt_senddata(const char* buffer, int n);
void bt_mode(btMode_t mode);

void bt_test(void);

#endif /* HW_BT_H_ */
