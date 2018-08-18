/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include <hw/debug.h>
#include "stm32f10x.h"

#include "hw/btn.h"
#include "hw/debug.h"
#include "hw/dma.h"
#include "hw/led.h"
#include "hw/oled.h"
#include "hw/time.h"

#include "os/api.h"
#include "os/loader.h"
#include "os/program_struct.h"
#include "os/program.h"
#include "os/shell.h"

#include "test.h"

int main(void)
{
	// Hardware init
	clock_init();
	btn_init();
	debug_init();
	dma_init();
	led_init();
	oled_init();
	time_init();

	// System init
	api_init();
	shell_init();

	// Post-init init
	oled_initialize_screen();
	oled_start_screen_transmission();

	run_test();

	for(;;);
}
