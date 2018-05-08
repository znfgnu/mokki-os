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

	uint8_t x = 0xAA;
	uint32_t cntr = 0;

	uint8_t cols = 8;
	uint8_t rows = 3;

	while(1) {
		oled_clear_buffer();
		uint8_t maxj = OLED_WIDTH / cols;
		for (int page=0; page<8; ++page) {
			for(int col=0; col<cols; ++col) {
				for (int j=0; j<maxj; ++j) {
					uint8_t gray = j&1 ? x : ~x;
					uint8_t color = (col-page/(8/rows))&7;
					switch (color){
					// white
					case 0:
					case 4:
						oled_buffer[page][col*maxj+j] = 0xff;
						break;
					// black
					case 2:
					case 6:
						oled_buffer[page][col*maxj+j] = 0x00;
						break;
					// checked gray
					case 1:
						oled_buffer[page][col*maxj+j] = gray;
						break;
					// whole square alternate gray
					case 3:
						oled_buffer[page][col*maxj+j] = ((cntr)&1) ? 0xff : 0x00;
						break;
					// vertical stripes alternate
					case 5:
						oled_buffer[page][col*maxj+j] = ((cntr^j)&1) ? 0xff : 0x00;
						break;
					// horizontal stripes alternate
					case 7:
						oled_buffer[page][col*maxj+j] = x;//((cntr)&1) ? 0xff : 0x00;
						break;
					}
				}
			}
		}
		cntr++;
		x=~x;
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
