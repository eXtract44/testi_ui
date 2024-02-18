#include "hardware.h"

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim12; //for buzzer

//---------------------ini values for menu settings
float map_hard(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void delay_mili(uint32_t __IO mili) {
	uint32_t i;
	mili *= 10000;
	for (i = 0; i < mili; i++)
		;
}
void turn_on_buttons() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0);
}
void turn_off_buttons() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 1);
}
void start_buzzer(uint16_t time, uint16_t volume) {
	uint16_t freq;
	if (volume > 100)
		volume = 100;
	if (time > 1000)
		time = 1000;
	freq = map_hard(volume, 1, 100, 100, 50000);
	__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, freq);
	delay_mili(time);
	__HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, 0);
}
void start_vibration(uint16_t time) {
	if (time > 100)
		time = 100;
	uint16_t temp = map_hard(time, 1, 100, 20, 70);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
	delay_mili(temp);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
}
void run_test_led(uint8_t status) {
	HAL_GPIO_WritePin(GPIOB, LED_Pin, status);
}
void turn_on_24v_internal() {
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, 1);
}
void turn_off_24v_internal() {
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, 0);
}
void turn_on_24v_external() {
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, 1);
}
void turn_off_24v_external() {
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, 0);
}
void toggle_24v_external() {
	HAL_GPIO_TogglePin(GPIOB, V24ON2_Pin);
}
void toggle_24v_internal() {
	HAL_GPIO_TogglePin(GPIOB, V24ON1_Pin);
}

