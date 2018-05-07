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
#include "os/uart.h"
#include "os/oled.h"

#include "examples/program.h"

int main(void)
{
	dma_init();
	uart_init();
	api_init();
	shell_init();
	oled_init();
	oled_initialize_screen();

	uint8_t x = 0xa5;
	uint32_t cntr = 0;

	while(1) {
		oled_clear_buffer();
		for(int i=0; i<8; ++i) {
			for (int j=0; j<8; ++j) {
				oled_buffer[i][8*i+j] = x;
			}
		}
		oled_buffer[7][127] = cntr++ / 100;
		uint8_t column = cntr & 0x7f;
		oled_buffer[3][column] = 0xff;
		x^=0xff;
		oled_update_screen();
	}

	uart_tx("Mokki development board");
	uart_tx("Version: 0.0.1a");

	while(1) {
		while (!uart_rx_flag);
		shell_parse(uart_rx_buffer);
		uart_rx_flag = 0;
	}

	for(;;);
}
