/*
 * oled.h
 *
 *  Created on: 03.05.2018
 *      Author: konrad
 */

#ifndef OS_OLED_H_
#define OS_OLED_H_

#include "stm32f10x.h"

#define OLED_DMA_Channel_IRQn	DMA1_Channel6_IRQn
#define OLED_DMA_Channel		DMA1_Channel6
#define OLED_DMA_FLAG			DMA1_FLAG_TC6

#define OLED_WIDTH		128
#define OLED_PAGES		8
#define OLED_HEIGHT		64
#define OLED_FRAME_LENGTH	(OLED_PAGES*OLED_WIDTH)

#define OLED_ADDR			0x78
// Tokens sent at the beginning of transmission.
#define OLED_DATA_TOKEN		0x40
#define OLED_CMD_TOKEN		0x00
// Special value indicating that DMA completed transfer.
#define OLED_TOKEN_TC_FLAG	0xff

typedef uint8_t oled_page_t[OLED_WIDTH];
typedef oled_page_t oled_buffer_t[OLED_PAGES];
extern oled_page_t *oled_buffer_tx;

void oled_init(void);
void oled_initialize_screen(void);
void oled_clear_buffer(void);
void oled_update_screen(void);

#endif /* OS_OLED_H_ */
