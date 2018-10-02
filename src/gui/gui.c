/*
 * gui.c
 *
 *  Created on: 22.09.2018
 *      Author: konrad
 */

#include "gfx/gfx.h"
#include "hw/btn.h"
#include "hw/time.h"
#include "os/program_struct.h"
#include <stdio.h>

#define NEXTMOD(a, m)	((a)+1==(m) ? 0 : (a)+1)
#define PREVMOD(a, m)	((a)==0 ? (m)-1 : (a)-1)

#define MAX(a,b)		((a)>(b)?(a):(b))
#define MIN(a,b)		((a)<(b)?(a):(b))

uint16_t breath_program[] = {
		// Program code
		0xe01e,
		0x4,
		0xf3af,
		0x8000,
		0xf3af,
		0x8000,
		0xf3af,
		0x8000,
		0xb580,
		0x4f01,
		0x47b8,
		0xbd80,
		0x1,
		0x0,
		0xb580,
		0x4f01,
		0x47b8,
		0xbd80,
		0x2,
		0x0,
		0xb580,
		0x4f01,
		0x47b8,
		0xbd80,
		0x3,
		0x0,
		0xb580,
		0x4f01,
		0x47b8,
		0xbd80,
		0x4,
		0x0,
		0xb538,
		0x2501,
		0x462c,
		0xe00d,
		0xb18c,
		0x2c80,
		0xbf08,
		0xf04f,
		0x35ff,
		0x442c,
		0xb2e2,
		0x4610,
		0x4611,
		0xf7ff,
		0xffe5,
		0x200a,
		0xf7ff,
		0xffe8,
		0x2000,
		0xf7ff,
		0xffd9,
		0x2800,
		0xd0ec,
		0x2000,
		0xbd38,
		0xbf00,
};

typedef int (*function_t)(void);
program_t launcher;

extern char _ebss;

int run_program(void) {
	loader_init_fntable((program_struct_t*) (&_ebss));
	program_t program = loader_get_launcher((program_struct_t*) (&_ebss));
	gfx_clear();
	gfx_update();
	int ret = program(0, 0);
	return ret;
}

int nothing(void) {
	return 0;
}

static const char* entries[] = {
		"Run program",
		"About",
};

static function_t actions[] = {
		&run_program,
		&nothing,
};

static const char* entries_helptext[] = {
		"Runs program stored in RAM.",
		"Visit https://mokki.org/"
};

static uint8_t appsno = sizeof(entries) / sizeof(entries[0]);

static uint8_t chosenno = 0;

static char buf[100];

void gui_start(void) {
	gfx_set_clear_color(COLOR_BLACK);
	uint8_t last_btn = btn_get_all();
	uint8_t btn = last_btn;

	uint32_t cntr = 0;
	uint32_t fps_cntr = 0;

	uint32_t fps = 0;

	uint32_t millis_now = time_now;
	uint32_t millis_last = millis_now;
	uint32_t lastchange = millis_now;

	while(1) {
		++cntr;
		++fps_cntr;
		// get input
		last_btn = btn;
		btn = btn_get_all();
		uint8_t pressed = ~((last_btn)|(~btn));
		millis_last = millis_now;
		millis_now = time_now;

		// logic
		if (pressed & BTN_DOWN) {
			chosenno = NEXTMOD(chosenno, appsno);
			lastchange = millis_now;
		}
		if (pressed & BTN_UP) {
			chosenno = PREVMOD(chosenno, appsno);
			lastchange = millis_now;
		}
		if (pressed & BTN_A) {
			actions[chosenno]();
		}
		if (millis_now/1000 != millis_last/1000) {
			fps = fps_cntr;
			fps_cntr = 0;
		}

		int8_t bulletpos = cntr & 0xF;
		bulletpos -= 8;
		bulletpos = bulletpos < 0 ? -bulletpos-1 : bulletpos;

		int helptext_offset = 0;
		int dt = millis_now - lastchange;
		if (dt > 1000) {
			helptext_offset = -(dt-1000)/50;
		}

		// draw
		gfx_clear();

		int bulletoff = strlen(entries[chosenno])*6 + 3 + 2;
		for (int i=0; i<3; ++i) {
			gfx_set_pixel(i, chosenno*8+bulletpos, COLOR_DARKGRAY);
			gfx_set_pixel(i+bulletoff, chosenno*8+bulletpos, COLOR_DARKGRAY);
		}

		for (int i=0; i<appsno; ++i) {
			if (i == chosenno) {
				font_print_string(4, i*8, entries[i], COLOR_WHITE);
			} else {
				font_print_string(0, i*8, entries[i], COLOR_DARKGRAY);
			}
		}

		sprintf(buf, " Total: %d b", raminfo_total());
		font_print_string(1, 3*8, buf, COLOR_DARKGRAY);
		sprintf(buf, "Static: %d b", raminfo_static());
		font_print_string(1, 4*8, buf, COLOR_DARKGRAY);
		sprintf(buf, " Stack: %d b", raminfo_stack());
		font_print_string(1, 5*8, buf, COLOR_DARKGRAY);
		sprintf(buf, "  Free: %d b", raminfo_free());
		font_print_string(1, 6*8, buf, COLOR_DARKGRAY);

		for (int i=0; i<8; ++i) {
			int col = btn & (1<<i) ? COLOR_WHITE : COLOR_DARKGRAY;
			font_blit_char(OLED_WIDTH-48+i*6, 7*8, '0'+i, col);
		}

		sprintf(buf, "%d FPS:%d", millis_now/1000, fps);
		font_print_string(1, 7*8, buf, COLOR_DARKGRAY);

//		gfx_fill_rect...
		font_print_string(helptext_offset, 2*8, entries_helptext[chosenno], COLOR_LIGHTGRAY);

		gfx_update();
	}
}
