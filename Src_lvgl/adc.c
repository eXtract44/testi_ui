/*
 * adc.c
 *
 *  Created on: 22.07.2021
 *      Author: O. Onopriienko
 */
#include "adc.h"
#include "stm32f4xx_hal.h"
#include "moving-median.h"

//--- values for adc dac

volatile uint16_t adc_data[NUMBER_OF_CHANNELS];
float u[NUMBER_OF_CHANNELS];
extern DAC_HandleTypeDef hdac;
//--- create med filter for ADC
movingMedian_t adc_filter_y,adc_filter_in1,adc_filter_in2,adc_filter_pt100,adc_filter_temp,adc_filter_bat; // adc_filter_24v;

float map_adc(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int8_t read_adc_mcuTemp() {
	u[ADC_CHANNEL_MCU_TEMPERATURE] = (float) adc_filter_temp.filtered * Vref / adcRes;
	u[ADC_CHANNEL_MCU_TEMPERATURE] = (u[ADC_CHANNEL_MCU_TEMPERATURE] - 0.76) / 0.0025 + 25;
	return (int8_t) u[ADC_CHANNEL_MCU_TEMPERATURE];
	// Average slope - 2.5
	//Voltage at 25 °C - 0.76
}

float adc_in_voltage(float input_value){
	float voltage = input_value * Vref / adcRes;
	voltage = voltage / (R2 / (R1 + R2));
	return voltage;
}
float read_adc_battery() {
	u[ADC_CHANNEL_BATTERY] = adc_in_voltage((float) adc_filter_bat.filtered);
	u[ADC_CHANNEL_BATTERY] = ((u[ADC_CHANNEL_BATTERY] - BATTERY_LOW_VOLTAGE) * (100 / (BATTERY_FULL_VOLTAGE - BATTERY_LOW_VOLTAGE)));
	if (u[ADC_CHANNEL_BATTERY] < 0) {
		u[ADC_CHANNEL_BATTERY] = 1;
	}
	if (u[ADC_CHANNEL_BATTERY] > 100) {
		u[ADC_CHANNEL_BATTERY] = 100;
	}
	return u[ADC_CHANNEL_BATTERY];
}
float read_adc_y() {
	u[ADC_CHANNEL_Y] = adc_in_voltage((float) adc_filter_y.filtered);
	return u[ADC_CHANNEL_Y];
}
float read_adc_in1() {
	u[ADC_CHANNEL_IN1] = adc_in_voltage((float) adc_filter_in1.filtered);
	return u[ADC_CHANNEL_IN1];
}
uint16_t read_adc_in1_raw() {
	uint16_t temp = adc_data[ADC_CHANNEL_IN1];
	return temp;
}
uint16_t read_adc_in1_potentiometer() {
	uint16_t temp = adc_filter_in1.filtered;
	temp = map_adc(temp, POTENTIOMETR_ADC_MIN, POTENTIOMETR_ADC_MAX, 0, 100);
	return temp;
}
float read_adc_in2() {
	u[ADC_CHANNEL_IN2] = adc_in_voltage((float) adc_filter_in2.filtered);
	return u[ADC_CHANNEL_IN2];
}
float read_adc_pt100() {
	u[ADC_CHANNEL_PT100] = (float) adc_filter_pt100.filtered;
	u[ADC_CHANNEL_PT100] = map_adc(u[ADC_CHANNEL_PT100], PT100_ADC_MIN,
	PT100_ADC_MAX, PT100_TEMP_MIN, PT100_TEMP_MAX) - PT100_ERROR;
	return u[ADC_CHANNEL_PT100];
}
void set_y(uint8_t volt) {
	if(volt < 0)volt = 0;
	if(volt>12)volt = 12;
	switch (volt) {
	case 0:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
		break;
	case 1:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC1V);
		break;
	case 2:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC2V);
		break;
	case 3:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC3V);
		break;
	case 4:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC4V);
		break;
	case 5:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC5V);
		break;
	case 6:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC6V);
		break;
	case 7:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC7V);
		break;
	case 8:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC8V);
		break;
	case 9:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC9V);
		break;
	case 10:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC10V);
		break;
	case 11:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC11V);
		break;
	case 12:
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC12V);
		break;
	}
}

void ini_adc_filter() {
	moving_median_create(&adc_filter_pt100, 30, 100);
//moving_median_create(&adc_filter_24v, filter_size_low, filter_time_low);
	moving_median_create(&adc_filter_y, 10, 50);
	moving_median_create(&adc_filter_in1, 10, 50);
	moving_median_create(&adc_filter_in2, 10, 50);
	moving_median_create(&adc_filter_temp, 50, 100);
	moving_median_create(&adc_filter_bat, 50, 100);
}
void adc_filter_loop() {
	moving_median_filter(&adc_filter_pt100, adc_data[ADC_CHANNEL_PT100]);
	moving_median_filter(&adc_filter_y, adc_data[ADC_CHANNEL_Y]);
	moving_median_filter(&adc_filter_in1, adc_data[ADC_CHANNEL_IN1]);
	moving_median_filter(&adc_filter_in2, adc_data[ADC_CHANNEL_IN2]);
	moving_median_filter(&adc_filter_temp, adc_data[ADC_CHANNEL_MCU_TEMPERATURE]);
	moving_median_filter(&adc_filter_bat, adc_data[ADC_CHANNEL_BATTERY]);
}

