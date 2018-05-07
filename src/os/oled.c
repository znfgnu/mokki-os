/*
 * oled.c
 *
 *  Created on: 03.05.2018
 *      Author: konrad
 */

#include "stm32f10x_i2c.h"
#include "stm32f10x_dma.h"
#include "os/oled.h"

// DOUBLE BUFFERING *.*
oled_buffer_t lcd_buffers[2];
oled_page_t* oled_buffer = &(lcd_buffers[0][0]);

const uint8_t oled_start_sequence[] = {
		0xAE, //display off
		0x20, //Set Memory Addressing Mode
		0x00, //00,Horizontal Addressing Mode,01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
		0xB0, //Set Page Start Address for Page Addressing Mode,0-7
		0xC8, //Set COM Output Scan Direction
		0x00, //---set low column address
		0x10, //---set high column address
		0x40, //--set start line address
		0x81, //--set contrast control register
		0xFF,
		0xA1, //--set segment re-map 0 to 127
		0xA6, //--set normal display
		0xA8, //--set multiplex ratio(1 to 64)
		0x3F, //
		0xA4, //0xa4,Output follows RAM content,0xa5,Output ignores RAM content
		0xD3, //-set display offset
		0x00, //-not offset
		0xD5, //--set display clock divide ratio/oscillator frequency
		0xF0, //--set divide ratio
		0xD9, //--set pre-charge period
		0x22, //
		0xDA, //--set com pins hardware configuration
		0x12,
		0xDB, //--set vcomh
		0x20, //0x20,0.77xVcc
		0x8D, //--set DC-DC enable
		0x14, //
		0xAF, //--turn on SSD1306 panel
		0xB0,
		0x00,
		0x10,
};

uint8_t oled_transfer_in_progress = 0;
void* oled_dma_data_ptr;
uint32_t oled_dma_data_size;
uint8_t oled_dma_token;
uint8_t oled_dma_tc;

void oled_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); // PB6-7 -> LCD_I2C_PORT pins pack 1

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD; // alternate function (I2C here)
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			// don't care when master
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C_InitStruct);
	I2C_Cmd(I2C1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}


void oled_dma_tx(const void* ptr, uint32_t size, uint8_t token) {
	/*
	 * Interrupt driven. Only set variables and generate start bit.
	 * Interrupts will do the work and reset oled_transfer_in_progress
	 * flag when complete.
	 */

	while(oled_transfer_in_progress);	// Wait until last transfer finishes.
	oled_transfer_in_progress = 1;
	oled_dma_data_ptr = ptr;
	oled_dma_data_size = size;
	oled_dma_token = token;
	I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
	I2C_GenerateSTART(I2C1, ENABLE);
}

void oled_initialize_screen(void) {
	oled_dma_tx(oled_start_sequence, sizeof(oled_start_sequence), OLED_CMD_TOKEN);
}

void oled_update_screen(void) {
	oled_dma_tx(oled_buffer, 1024, OLED_DATA_TOKEN);
	// Swap buffers.
	oled_buffer = (oled_buffer == &lcd_buffers[0][0]) ? &lcd_buffers[1][0] : &lcd_buffers[0][0];
}

void oled_clear_buffer(void) {
	for (int i = 0; i < OLED_PAGES; ++i) {
		for (int j = 0; j < OLED_WIDTH; ++j) {
			oled_buffer[i][j] = 0x00;
		}
	}
}

// Interrupt handlers

void I2C1_EV_IRQHandler(void) {
	switch (I2C_GetLastEvent(I2C1)) {
	case I2C_EVENT_MASTER_MODE_SELECT:
		I2C_Send7bitAddress(I2C1, OLED_ADDR, I2C_Direction_Transmitter);
		break;
	case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
		I2C_SendData(I2C1, oled_dma_token);
		break;
	case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
		if (oled_dma_token == OLED_TOKEN_TC_FLAG) {
			I2C_GenerateSTOP(I2C1, ENABLE);
			I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
			oled_transfer_in_progress = 0;
		}
		else {
			// Disable I2C interrupt
			I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
			// Disable DMA before clearing interrupt flags
			OLED_DMA_Channel->CCR = 0;	// Control register
			// Clear interrupts
			DMA1->IFCR |= ((uint32_t)(DMA_ISR_GIF6 | DMA_ISR_TCIF6 | DMA_ISR_HTIF6 | DMA_ISR_TEIF6));	// Interrupt flag clear register
			// Init DMA
			OLED_DMA_Channel->CCR = \
					DMA_DIR_PeripheralDST | \
					DMA_Mode_Normal | \
					DMA_PeripheralInc_Disable | \
					DMA_MemoryInc_Enable | \
					DMA_PeripheralDataSize_Byte | \
					DMA_MemoryDataSize_Byte | \
					DMA_Priority_VeryHigh | \
					DMA_M2M_Disable;
			OLED_DMA_Channel->CMAR = (uint32_t) oled_dma_data_ptr;	// Memory address
			OLED_DMA_Channel->CPAR = (uint32_t) &(I2C1->DR);	// Peripheral address
			OLED_DMA_Channel->CNDTR = oled_dma_data_size;		// Remaining bytes
			// Turn on DMA request
			I2C1->CR2 |= I2C_CR2_DMAEN;
			// Enable DMA interrupt
			DMA_ITConfig(OLED_DMA_Channel, DMA_IT_TC, ENABLE);
			// Turn on DMA
			OLED_DMA_Channel->CCR |= DMA_CCR1_EN;
		}
		break;
	}
}

void DMA1_Channel6_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_IT_TC6)) {
		DMA_ClearITPendingBit(DMA1_IT_TC6);
		oled_dma_token = OLED_TOKEN_TC_FLAG;
		I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
		DMA_ITConfig(OLED_DMA_Channel, DMA_IT_TC, DISABLE);
		DMA_Cmd(OLED_DMA_Channel, DISABLE);
		I2C_DMACmd(I2C1, DISABLE);
	}
}
