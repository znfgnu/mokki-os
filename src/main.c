/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f10x.h"

#include "os/api.h"
#include "os/loader.h"
#include "os/program_struct.h"
#include "os/shell.h"
#include "os/uart.h"

#include "examples/program.h"

int main(void)
{
	uart_init();
	api_init();
	shell_init();

	uart_tx("Mokki development board");
	uart_tx("Version: 0.0.1a");

	while(1) {
		while (!uart_rx_flag);
		shell_parse(uart_rx_buffer);
		uart_rx_flag = 0;
	}

	for(;;);
}
