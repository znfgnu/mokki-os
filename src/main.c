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

//	uart_tx("Mokki development board");
//	uart_tx("Version: 0.0.1a");

	uint32_t cntr = 0;

//	gfx_test();

	while(1) {
		for (int l=0; l<8; ++l) {
			for (int c=0; c<128; ++c) {
				if (((l+c/8)&3) == 0) {	// black
					oled_buffer[l][c] = 0x00;
				} else if (((l+c/8)&3) == 1) {	// gray dark
//					uint8_t res = (c+cntr)%4;
//					switch(res) {
//					case 3:
//						res = 0x49;	// 3 bits
//						break;
//					case 1:
//						res = 0x92;	// 3 bits
//						break;
//					case 0:
//					case 2:
//						res = 0x24;	// 2 bits - darker
//						break;
//					}
					uint8_t res = c<64 ? (cntr+c)%3 : (cntr-c)%3;
					switch(res) {
					case 0:
					case 4:
						res = 0xAA;//c&1 ? 0xAA : 0x55;
						break;
					case 1:
					case 3:
						res = 0x55;//c&1 ? 0x55 : 0xAA;
						break;
					case 2:
					case 5:
						res = 0x00;
						break;
					}

					oled_buffer[l][c] = res;//(((c+cntr)%3) == 0) ? 0xff : 0x00;
				} else if (((l+c/8)&3) == 2) {	// gray light
//					uint8_t res = (c+cntr)%4;
//					switch(res) {
//					case 3:
//						res = 0xb6; // 3 bits off
//						break;
//					case 1:
//						res = 0x6d;	// 3 bits off
//						break;
//					case 0:
//					case 2:
//						res = 0xdb; // 2 bits off - brighter
//						break;
//					}
					uint8_t res = c<64 ? (cntr+c)%3 : (cntr-c)%3;
					switch(res) {
					case 0:
					case 4:
						res = 0xAA;//c&1 ? 0xAA : 0x55;
						break;
					case 1:
					case 3:
						res = 0x55;//c&1 ? 0x55 : 0xAA;
						break;
					case 2:
					case 5:
						res = 0xff;
						break;
					}

					oled_buffer[l][c] = res;//(((c+cntr)%3) != 0) ? 0xff : 0x00;
				} else {	// white
					oled_buffer[l][c] = 0xff;//0xff;
				}
			}
		}
//		cntr = (cntr + 1 == 3) ? 0 : (cntr + 1);
		++cntr;
//		oled_dma_tx(freeze, 1, OLED_CMD_TOKEN);
		oled_update_screen();
//		oled_dma_tx(unfreeze, 1, OLED_CMD_TOKEN);
//		while (!uart_rx_flag);
//		shell_parse(uart_rx_buffer);
//		uart_rx_flag = 0;
	}

	for(;;);
}
