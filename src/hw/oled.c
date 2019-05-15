/*
 * oled.c
 *
 *  Created on: 03.05.2018
 *      Author: konrad
 */

#include "stm32f10x_i2c.h"
#include "stm32f10x_dma.h"
#include "hw/oled.h"

oled_buffer_t oled_triple_buffer[2][3];

const uint8_t oled_start_sequence[] = {
		0xAE,	// Set display OFF

		0x20,	// Set memory addressing mode:
		0x00,	// Horizontal: 0x00, Vertical: 0x01, Page (default): 0x02

		0x21,	// Set column address
		0,		// start
		127,	// end

		0x22,	// Set page address
		0,		// start
		7,		// end

		0xC8,	// Set COM Output Scan Direction (0xC0 - flipped vertical)

		0x40,	// Set display start line to 0

		0x81,	// Set contrast control register
//		0x3f,	// Value: 0-255
		0x7f,	// Value: 0-255
//		0xff,	// Value: 0-255

		0xA1, 	// Set segment re-map 0 to 127 (0xA0 - flipped horizontal)

		0xA6, 	// Set normal display (inverted colors: 0xA7)

		0xA8,	// Set multiplex ratio
		0x3F, 	// 15 (16MUX) to 63 (64MUX)

		0xA4, 	// Output follows RAM content; 0xA5: Output ignores RAM content (all pixels lit up)

		0xD3,	// Set display offset (vertical)
		0x00,	// 0 to 63

		0xD5,	// Set display clock divide ratio / oscillator frequency
//		0xD0,	// TEST1	// 4 MSB: oscillator frequency; 4 LSB: divide ratio of the display clocks
		0xF0,	// TEST0	// 4 MSB: oscillator frequency; 4 LSB: divide ratio of the display clocks
//		0x0F,	// 4 MSB: oscillator frequency; 4 LSB: divide ratio of the display clocks

		0xD9,	// Set pre-charge period
		0x41,	// 4 MSB: Phase 2 period of up to 15 DCLK; 4 LSB: Phase 1 period of up to 15 DCLK

		0xDA, //--set com pins hardware configuration
		0x12,

		0xDB, //--set vcomh
		0x20, //0x20,0.77xVcc

		0x8D,	// Charge pump setting
		0x14, 	// Enable: 0x14, Disable: 0x10

		0xAF, 	// Display ON
};

uint8_t oled_transfer_in_progress = 0;
void* oled_dma_data_ptr;
uint32_t oled_dma_data_size;
uint8_t oled_dma_circular = 0;

uint8_t volatile oled_free_buffer = 1;
uint8_t volatile oled_request_flip = 0;

uint8_t oled_dma_token;
uint8_t oled_dma_tc;

extern uint8_t rebootRequested;

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

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

	I2C_InitStruct.I2C_ClockSpeed = 943000;	// TEST1
//	I2C_InitStruct.I2C_ClockSpeed = 898890;	// TEST0
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


void oled_dma_tx(const void* ptr, uint32_t size, uint8_t token, uint8_t circular) {
	/*
	 * Interrupt driven. Only set variables and generate start bit.
	 * Interrupts will do the work and reset oled_transfer_in_progress
	 * flag when complete.
	 */

	while(oled_transfer_in_progress) __WFI();	// Wait until last transfer finishes.
	oled_transfer_in_progress = 1;
	oled_dma_data_ptr = ptr;
	oled_dma_data_size = size;
	oled_dma_token = token;
	oled_dma_circular = circular;
	I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
	I2C_GenerateSTART(I2C1, ENABLE);
}

void oled_initialize_screen(void) {
	oled_dma_tx(oled_start_sequence, sizeof(oled_start_sequence), OLED_CMD_TOKEN, 0);
}

void oled_start_screen_transmission(void) {
	oled_dma_tx(&oled_triple_buffer[0], 3 * sizeof(oled_buffer_t), OLED_DATA_TOKEN, 1);
}

void spawn_dma(void) {
	// Disable DMA before clearing interrupt flags
	OLED_DMA_Channel->CCR = 0;	// Control register
	// Clear interrupts
	DMA1->IFCR |= ((uint32_t)(DMA_ISR_GIF6 | DMA_ISR_TCIF6 | DMA_ISR_HTIF6 | DMA_ISR_TEIF6));	// Interrupt flag clear register
	// Init DMA
	OLED_DMA_Channel->CCR = \
			DMA_DIR_PeripheralDST | \
			DMA_PeripheralInc_Disable | \
			DMA_MemoryInc_Enable | \
			DMA_PeripheralDataSize_Byte | \
			DMA_MemoryDataSize_Byte | \
			DMA_Priority_VeryHigh | \
			DMA_M2M_Disable;
	OLED_DMA_Channel->CCR |= DMA_Mode_Normal;	//oled_dma_circular ? DMA_Mode_Circular : DMA_Mode_Normal;
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
			I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
			oled_transfer_in_progress = 0;
		}
		else {
			// Disable I2C interrupt
			I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);
			spawn_dma();
		}
		break;
	}
}

void DMA1_Channel6_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_IT_TC6)) {
		DMA_ClearITPendingBit(DMA1_IT_TC6);
		if (rebootRequested) {
			oled_transfer_in_progress = 0;
			return;
		}

		if (oled_dma_circular) {
			if (oled_request_flip) {
				oled_dma_data_ptr = &oled_triple_buffer[oled_free_buffer];
				oled_free_buffer ^= 1;
				oled_request_flip = 0;
			}
			spawn_dma();

		}
		else {
			oled_dma_token = OLED_TOKEN_TC_FLAG;
			I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
			DMA_ITConfig(OLED_DMA_Channel, DMA_IT_TC, DISABLE);
			DMA_Cmd(OLED_DMA_Channel, DISABLE);
			I2C_DMACmd(I2C1, DISABLE);
		}
	}
}
