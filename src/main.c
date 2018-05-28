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
#include "os/dma.h"
#include "hw/uart.h"
#include "hw/oled.h"
#include "hw/time.h"
#include "gfx/font.h"
#include "gfx/test.h"

#include "examples/program.h"

int main(void)
{
	dma_init();
	uart_init();
	api_init();
	shell_init();
	oled_init();
	oled_initialize_screen();
	time_init();

//	gfx_test();

	uart_tx("Mokki development board");
	uart_tx("Version: 0.0.1a");

	while(1) {
		while (!uart_rx_flag);
		shell_parse(uart_rx_buffer);
		uart_rx_flag = 0;
	}

	for(;;);
}
