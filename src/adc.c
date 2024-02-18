/*
 * adc.c
 *
 *  Created on: 22.02.2024
 *      Author: Oleksandr Onopriienko
 */
#include "adc.h"
#include "moving-median.h"

volatile uint16_t adc_data_raw[ADC_CHANNEL_QUANTITY];

movingMedian_t adc_filter[ADC_CHANNEL_QUANTITY];

int constrtain_int(int input_value,int min,int max){
int temp = 0;
if(input_value > max){
	temp = max;
}else{
	temp = min;
}
return temp;
}
float map_adc(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float read_adc_voltage(uint16_t adc_filtred_value){
	float temp = ((float)adc_filtred_value) * Vref / adcRes;
	temp = adc_filtred_value / (R2 / (R1 + R2));
	return temp;
}
void read_adc_allchannels() {
	ILI9341_setFont(&Font16);
	uint16_t xStart = 170;
	uint16_t yStart = 50;
	uint16_t ySize = 20;

	u[0] = (float) adc_data[0];
	sprintf(str, "%1.f ", u[0]);
	ILI9341_printText(str, xStart, yStart, COLOR_WHITE, COLOR_MENU_BG, 2);
	u[0] = ((float) adc_data[0]) * Vref / adcRes;
	sprintf(str, "%2.fV ", u[0]);
	ILI9341_printText(str, xStart + 80, yStart, COLOR_WHITE, COLOR_MENU_BG, 2);

	u[1] = (float) adc_data[1];
	sprintf(str, "%1.f ", u[1]);
	ILI9341_printText(str, xStart, yStart + ySize, COLOR_WHITE, COLOR_MENU_BG,
			2);
	u[1] = u[1] * Vref / adcRes;
	u[1] = u[1] / (R3 / (R1 + R3));
	sprintf(str, "%2.fV ", u[1]);
	ILI9341_printText(str, xStart + 80, yStart + ySize, COLOR_WHITE,
			COLOR_MENU_BG, 2);

	u[2] = (float) adc_data[2];
	sprintf(str, "%1.f ", u[2]);
	ILI9341_printText(str, xStart, yStart + ySize * 2, COLOR_WHITE,
			COLOR_MENU_BG, 2);
	u[2] = (float) adc_data[2] * Vref / adcRes;
	u[2] = u[2] / (R2 / (R1 + R2));
	sprintf(str, "%2.fV ", u[2]);
	ILI9341_printText(str, xStart + 80, yStart + ySize * 2, COLOR_WHITE,
			COLOR_MENU_BG, 2);

	u[3] = (float) adc_data[3];
	sprintf(str, "%1.f ", u[3]);
	ILI9341_printText(str, xStart, yStart + ySize * 3, COLOR_WHITE,
			COLOR_MENU_BG, 2);
	u[3] = ((float) adc_data[3]) * Vref / adcRes;
	u[3] = u[3] / (R2 / (R1 + R2));
	sprintf(str, "%2.fV ", u[3]);
	ILI9341_printText(str, xStart + 80, yStart + ySize * 3, COLOR_WHITE,
			COLOR_MENU_BG, 2);

	u[4] = (float) adc_data[4];
	sprintf(str, "%1.f ", u[4]);
	ILI9341_printText(str, xStart, yStart + ySize * 4, COLOR_WHITE,
			COLOR_MENU_BG, 2);
	u[4] = (float) adc_data[4] * Vref / adcRes;
	u[4] = u[4] / (R2 / (R1 + R2));
	sprintf(str, "%2.fV ", u[4]);
	ILI9341_printText(str, xStart + 80, yStart + ySize * 4, COLOR_WHITE,
			COLOR_MENU_BG, 2);

	u[5] = (float) adc_data[5];
	sprintf(str, "%1.f ", u[5]);
	ILI9341_printText(str, xStart, yStart + ySize * 5, COLOR_WHITE,
			COLOR_MENU_BG, 2);

	u[6] = (float) adc_data[6];
	sprintf(str, "%1.f ", u[6]);
	ILI9341_printText(str, xStart, yStart + ySize * 6, COLOR_WHITE,
			COLOR_MENU_BG, 2);
	u[6] = (float) adc_data[6] * Vref / adcRes;
	u[6] = u[6] / (R2 / (R1 + R2));
	sprintf(str, "%2.fV ", u[6]);
	ILI9341_printText(str, xStart + 80, yStart + ySize * 6, COLOR_WHITE,
			COLOR_MENU_BG, 2);

}
float read_mcu_temperature() {
	/* Average slope - 2.5
	Voltage at 25 °C - 0.76*/
	float temp = (float) adc_filter[ADC_CHANNEL_MCU_TEMPERATURE].filtered * Vref / adcRes;
	temp = (temp - 0.76f) / 0.0025f + 25.0f;
	return temp;
}

uint8_t read_battery() {
	/* Datasheet
	 Overcharge Detection Voltage: 4.25-4.35V±0.05V
	 Over Discharge Detection Voltage: 2.3-3.0V±0.05V
	 */
	const float Vmax = 8.405f; // = 100% Battery
	const float Vmin = 6.0f; // = 0% Battery
	float battery_charge_precent = ((read_adc_voltage(adc_filter[ADC_CHANNEL_BATTERY].filtered) - Vmin) * (100 / (Vmax - Vmin)));
	battery_charge_precent=constrtain_int((int)battery_charge_precent ,0,100);
	return (uint8_t)battery_charge_precent;
}
void ini_adc_filter() {
	uint8_t filter_size_high = 50;
	uint8_t filter_size_medium = 40;
	uint8_t filter_size_low = 40;

	uint16_t filter_time_high = 100;
	uint16_t filter_time_medium = 100;
	uint16_t filter_time_low = 100;

	moving_median_create(&adc_filter[ADC_CHANNEL_INPUT_PT100], filter_size_high, filter_time_high);
//moving_median_create(&adc_filter[ADC_CHANNEL_OUTPUT_24V], filter_size_low, filter_time_low);
	moving_median_create(&adc_filter[ADC_CHANNEL_OUTPUT_0_10V], filter_size_high, filter_time_high);
	moving_median_create(&adc_filter[ADC_CHANNEL_INPUT1_0_10V], filter_size_high, filter_time_high);
	moving_median_create(&adc_filter[ADC_CHANNEL_INPUT2_0_10V], filter_size_high, filter_time_high);
	moving_median_create(&adc_filter[ADC_CHANNEL_MCU_TEMPERATURE], filter_size_medium,
			filter_time_medium);
	moving_median_create(&adc_filter[ADC_CHANNEL_BATTERY], filter_size_low, filter_time_low);
}
void adc_filter_loop() {

	moving_median_filter(&adc_filter[ADC_CHANNEL_INPUT_PT100], adc_data[ADC_CHANNEL_INPUT_PT100]);
////moving_median_filter(&adc_filter[ADC_CHANNEL_OUTPUT_24V], adc_data[ADC_CHANNEL_OUTPUT_24V]);
	moving_median_filter(&adc_filter[ADC_CHANNEL_OUTPUT_0_10V], adc_data[ADC_CHANNEL_OUTPUT_0_10V]);
	moving_median_filter(&adc_filter[ADC_CHANNEL_INPUT1_0_10V], adc_data[ADC_CHANNEL_INPUT1_0_10V]);
	moving_median_filter(&adc_filter[ADC_CHANNEL_INPUT2_0_10V], adc_data[ADC_CHANNEL_INPUT2_0_10V]);
	moving_median_filter(&adc_filter[ADC_CHANNEL_MCU_TEMPERATURE], adc_data[ADC_CHANNEL_MCU_TEMPERATURE]);
	moving_median_filter(&adc_filter[ADC_CHANNEL_BATTERY], adc_data[ADC_CHANNEL_BATTERY]);
//moving_median_filter(&med_filter6, adc_data[6]);
}
