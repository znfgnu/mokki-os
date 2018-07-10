/*
 * font.h
 *
 *  Created on: 13.05.2018
 *      Author: konrad
 */

#ifndef GFX_FONT_H_
#define GFX_FONT_H_

#include "stm32f10x.h"

#define FONT_WIDTH			5
#define FONT_SPACING		1
#define FONT_WIDTH_TOTAL	(FONT_WIDTH+FONT_SPACING)
#define FONT_NO_CHARACTERS			255

typedef uint8_t font_character_t[FONT_WIDTH];
extern const font_character_t font_default[FONT_NO_CHARACTERS];

void font_blit_char(int x, int y, char c, int color);
void font_print_string(int x, int y, const char* str, int color);

#endif /* GFX_FONT_H_ */
