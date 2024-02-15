/*
 * processing.c
 *
 *  Created on: Sep 21, 2021
 *      Author: sasho
 */
//#include "processing.h"
#include "main.h"
#include "spi_ili9341.h"

//cnt for processing
uint8_t cnt_processing = 0;

extern TIM_HandleTypeDef htim12;

void start_buzzer(uint8_t status) {
	//uint16_t freq = 0;
	switch (status) {
	case 0:
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
		break;
	case 1:
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 700);
		break;
	case 2:
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 3000);
		break;
	case 3:
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 7000);
		break;
	case 4:
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 10000);
		break;
	default:
		__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
		break;
	}
}

void processing() {
	uint16_t xstart = 100;
	uint16_t ystart = 220;
	uint16_t size = 3;
	uint16_t xdistance = 10;
	HAL_GPIO_TogglePin(GPIOB, LED_Pin);

	cnt_processing++;
	if (cnt_processing == 1) {
		ILI9341_fillRect(xstart, ystart, xstart + size + xdistance * 2,
				ystart + size,
				COLOR_MENU_BAR_DOWN);
	}
	if (cnt_processing == 2) {
		ILI9341_fillRect(xstart, ystart, xstart + size, ystart + size,
		COLOR_BLUE);
	}
	if (cnt_processing == 3) {
		ILI9341_fillRect(xstart + xdistance, ystart, xstart + size + xdistance,
				ystart + size,
				COLOR_BLUE);
	}
	if (cnt_processing == 4) {
		ILI9341_fillRect(xstart + xdistance * 2, ystart,
				xstart + size + xdistance * 2, ystart + size,
				COLOR_BLUE);
		cnt_processing = 0;
	}

}
