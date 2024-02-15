/*
 * adc.c
 *
 *  Created on: 22.07.2021
 *      Author: Alex
 */
#include "adc.h"
#include "moving-median.h"
#include "main.h"
#include  <stdbool.h>
#include "spi_ili9341.h"
#include "menu.h"
#include  <stdint.h>
#include "processing.h"
#include "buttons.h"
#include "stm32f4xx_hal.h"
#include "time.h"
//--- values for adc dac
extern volatile uint16_t adc_data[];
extern float u[];
extern uint16_t DacVal;
extern uint8_t DacVolt;
extern DAC_HandleTypeDef hdac;
//--- creat med filter
movingMedian_t adc_filter_24v;
movingMedian_t adc_filter_y;
movingMedian_t adc_filter_in1;
movingMedian_t adc_filter_in2;
movingMedian_t adc_filter_pt100;
movingMedian_t adc_filter_temp;
movingMedian_t adc_filter_bat;
//values for position
extern uint16_t x_pos_button_l;
extern uint16_t y_pos_button_l; //

extern uint16_t x_pos_button_r; //247
extern uint16_t y_pos_button_r;

extern uint16_t x_pos_button_c;
extern uint16_t y_pos_button_c;

extern uint16_t x_pos_batt;
extern uint16_t y_pos_batt;
//values for printing
extern uint16_t frame_val_ystart;
extern uint16_t frame_val_ysize;
extern uint16_t frame_val_xstart;

extern uint16_t adc_val_xstart;
extern uint16_t adc_val_ystart;
extern uint16_t mess_val_xstart; //
//other values
extern uint8_t alarm_ton;
extern bool status_idle;
int16_t adc_pt100 = 0; // temp adc value for graphic
char str[20]; // for string
extern bool button_pressed; // button status
extern uint8_t buttons_l_r_status;
extern uint8_t button_select_status;
extern uint8_t test_menu;
//CNTs for menus func
uint32_t cnt_new_batt = 0;
uint32_t cnt_old_batt = 0;
uint16_t cnt_batt = 0;
uint32_t cnt_new_pt100 = 0;
uint32_t cnt_old_pt100 = 0;
uint32_t cnt_new_fault = 0;
uint32_t cnt_old_fault = 0;
// read adc in
bool running = false; // while status
uint8_t current_main_page = 1; //read ADC
uint8_t current_sensor_page = 0; //read ADC
uint8_t current_antrieb_page = 0; //read ADC
uint8_t current_info_page = 0; //read ADC
uint8_t current_pt100_page = 1;
// for DAC
bool value_matched = false;

//---------------------time values

uint16_t interval_gr = 24;  //24*0.5sec = 12 sec
uint16_t interval_bat = 300;
//values for DAC
uint16_t dac_time_old;
uint16_t dac_time_new;
//values for pt100 graphic
int16_t pt100_graphic_x_old;
int16_t pt100_graphic_y_old;
int16_t pt100_graphic_x_new;
int16_t pt100_graphic_y_new;
uint16_t cy_old = 0;
uint16_t cy_new = 0;
//---------------------ini values for submenu SENSOREN

uint8_t measuring_range_raumregler_aktiv = 1;
//uint8_t measuring_range_raumtemp = 1;
uint8_t measuring_range_kanaltemp = 1;
uint8_t measuring_range_feuchte = 1;
uint8_t measuring_range_druckluft = 1;
uint8_t measuring_range_helligkeit = 1;
int16_t yvalue_min = 0;
uint16_t yvalue_max = 0;
uint16_t multiply_min_max = 1;

float map(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void read_adc_allchannels() {
	ILI9341_setFont(&Font16);
	uint16_t xStart = 170;
	uint16_t yStart = 50;
	uint16_t ySize = 20;

//	u[adc_channel_y] = ((float) adc_filter_y.filtered) * Vref / adcRes;
//			u[adc_channel_y] = u[adc_channel_y] / (R2 / (R1 + R2));
//
//			sprintf(str, "%.2fV ", u[adc_channel_y]);
//			ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
//
//

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
void read_adc_mcuTemp() {
	//ILI9341_setFont(&Font16);
	u[adc_channel_temp] = (float) adc_filter_temp.filtered * Vref / adcRes;
	u[adc_channel_temp] = (u[adc_channel_temp] - 0.76) / 0.0025 + 25;
	sprintf(str, "%.1f * ", u[adc_channel_temp]);
	ILI9341_printText(str, 45, 17, COLOR_WHITE, COLOR_MENU_BAR_DOWN, 1);
	//temp = ((V25 - (ADC_DATA*Vref/adcRes)) / Avg_Slope) + 25
	// Average slope - 2.5
	//Voltage at 25 °C - 0.76
}
void read_adc_battery(uint16_t x, uint16_t y) {
	/* Datasheet
	 Overcharge Detection Voltage: 4.25-4.35V±0.05V
	 Over Discharge Detection Voltage: 2.3-3.0V±0.05V
	 */

	float Vmax = 8.405; // = 100% Battery
	float Vmin = 6.0; // = 0% Battery

	u[adc_channel_bat] = ((float) adc_filter_bat.filtered) * Vref / adcRes;
	u[adc_channel_bat] = u[adc_channel_bat] / (R2 / (R1 + R2));
	u[adc_channel_bat] = ((u[adc_channel_bat] - Vmin) * (100 / (Vmax - Vmin)));
	sprintf(str, "%3d", (uint16_t)u[adc_channel_bat]);
	if (u[adc_channel_bat] > 0 && u[adc_channel_bat] < 105) { //
		if (button_select_status == 1 || buttons_l_r_status == 7) {
			ILI9341_printText(str, x - 20, y + 3, COLOR_GREEN,
			COLOR_MENU_BAR_DOWN, 1);
		} else {
			ILI9341_printText(str, x - 20, y + 3, COLOR_GREEN, COLOR_MENU_BG,
					1);
		}
	}
	else {
		ILI9341_printText("LOAD", x_pos_batt+4, y_pos_batt - 22, COLOR_GREEN,
					COLOR_MENU_BAR_DOWN, 1);
	}
	ILI9341_drawRect(x - 1, y - 2, x + 31, y + 13, COLOR_WHITE);
	ILI9341_drawRect(x + 31, y + 1, x + 33, y + 10, COLOR_WHITE);
	if (u[adc_channel_bat] >= 10) {
		ILI9341_fillRect(x + 1, y, x + 5, y + 11, COLOR_GREEN);

	} //20%

	if (u[adc_channel_bat] <= 20) {
		cnt_batt++;
		if (cnt_batt > 90) { //1 tick = 10 sec
			ILI9341_fillRect(x + 1, y, x + 5, y + 11, COLOR_RED);
			start_buzzer(4);
			HAL_Delay(1000);
			start_buzzer(0);
			ILI9341_fillRect(x + 1, y, x + 5, y + 11, COLOR_GREEN);
			cnt_batt = 0;
		}

	}
	if (u[adc_channel_bat] >= 30) {
		ILI9341_fillRect(x + 7, y, x + 11, y + 11, COLOR_GREEN);
	} //40%
	if (u[adc_channel_bat] >= 50) {
		ILI9341_fillRect(x + 13, y, x + 17, y + 11, COLOR_GREEN);
	} //60%
	if (u[adc_channel_bat] >= 70) {
		ILI9341_fillRect(x + 19, y, x + 23, y + 11, COLOR_GREEN);
	} //80%
	if (u[adc_channel_bat] >= 90) {
		ILI9341_fillRect(x + 25, y, x + 29, y + 11, COLOR_GREEN);
	} //100%
}
void read_adc_home() {
	//ILI9341_setFont(&Font20);
	start_buzzer(0);
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "<<");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, ">>");
	draw_start_button(COLOR_WHITE);
	DacVal = 0; //set DAC to 0
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DacVal);
	read_adc_mcuTemp();
	get_time();
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_mcuTemp();
		read_adc_battery(x_pos_batt, y_pos_batt - 25);
	}
}
void read_adc_info() {
	//ILI9341_setFont(&Font24);
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "<<");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, ">>");
	draw_start_button(COLOR_WHITE);
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_mcuTemp();
		read_adc_battery(x_pos_batt, y_pos_batt - 25);
	}
	if (test_menu == 1) {
		read_adc_allchannels();
	}
}
void read_adc_antrieb_r() {
	//running = true;
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "Y-");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "Y+");
	//-----------------------------Y
	read_adc_channels_v(1, adc_val_xstart, adc_val_ystart);
	//-----------------------------IN1
	u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
	u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
	if (u[adc_channel_in1] > 12) {
		start_buzzer(alarm_ton);
		start_vibration();
	} else {
		start_buzzer(0);
	}
	sprintf(str, "%.2fV ", u[adc_channel_in1]);
	u[adc_channel_in1] = u[adc_channel_in1] * 10;
	draw_antriebAnim(95, 50, (int16_t) u[adc_channel_in1]);
	ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	//-----------------------------Ub
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_antrieb_s() {
	//running = true;
//	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
//	COLOR_MENU_BAR_DOWN, "INFO");
//	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
//	COLOR_MENU_BAR_DOWN, "<<<");
	//-----------------------------Bat
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC5V);

	//-----------------------------In1 AUF

	if (adc_data[adc_channel_in1] > 300) {
		ILI9341_printText("AUF", adc_val_xstart, adc_val_ystart, COLOR_WHITE,
		COLOR_MENU_BG, 2);
	} else {
		ILI9341_printText("   ", adc_val_xstart, adc_val_ystart, COLOR_WHITE,
		COLOR_MENU_BG, 2);
	}
	//-----------------------------In2 ZU
	if (adc_data[adc_channel_in2] > 300) {
		ILI9341_printText(" ZU", adc_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	} else {
		ILI9341_printText("   ", adc_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	//-----------------------------Ub
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}

	//running = false;
}
void read_adc_raumregler_aktiv() {
	//running = true;
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M-");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M+");
//-----------------------------in1 temp
	read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------in2 	soll
	read_adc_channels_p(3, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);
	//-----------------------------Ub
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 3);
	//-----------------------------Messbereich
	if (measuring_range_raumregler_aktiv == 1) {  //-50  +50
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 10 - 50;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" -50/50", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_raumregler_aktiv == 2) { //0/50
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 5;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/50 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_raumregler_aktiv == 3) { //-15 +35
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 5 - 15;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" -15/35", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);

	}

	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_raumregler_passiv() {
	//running = true;
	//DacVal = DAC10V; //set DAC
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC10V);
///**test Resistor ADC
//	u[adc_channel_in1] = (float) adc_filter_in1.filtered; //test ADC
//	sprintf(str, "%3d%% ", (uint16_t) u[adc_channel_in1]);
//	ILI9341_printText(str, adc_val_xstart, adc_val_ystart, COLOR_WHITE,
//	COLOR_MENU_BG, 2);
///**test Resistor ADC
//-----------------------------in1 poti
	u[adc_channel_in1] = (float) adc_filter_in1.filtered;
	u[adc_channel_in1] = map(u[adc_channel_in1], POTI_ADC_MIN, POTI_ADC_MAX, 0,
			100);
	if (u[adc_channel_in1] > 120) {
		ILI9341_printText("Poti fehlt", mess_val_xstart - 50, adc_val_ystart,
		COLOR_RED, COLOR_MENU_BG, 1);
	} else {
		ILI9341_printText("          ", mess_val_xstart - 50, adc_val_ystart,
		COLOR_WHITE, COLOR_MENU_BG, 1);
		sprintf(str, "%3d%% ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart, COLOR_WHITE,
		COLOR_MENU_BG, 2);
	}
	///**test PT100 ADC
//	u[adc_channel_pt100] = (float) adc_filter_pt100.filtered;
//	sprintf(str, "%.1f* ", u[adc_channel_pt100]);
//			ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
//			COLOR_WHITE,
//			COLOR_MENU_BG, 2);
	///**test PT100 ADC
//-----------------------------pt100
	u[adc_channel_pt100] = (float) adc_filter_pt100.filtered;
	u[adc_channel_pt100] = map(u[adc_channel_pt100], PT100_ADC_MIN,
	PT100_ADC_MAX, PT100_TEMP_MIN, PT100_TEMP_MAX) - PT100_ERROR; // new
	if (u[adc_channel_pt100] > 200 || u[adc_channel_pt100] < -20) {
		ILI9341_printText("pt100 fehlt", mess_val_xstart - 50,
				adc_val_ystart + frame_val_ysize, COLOR_RED, COLOR_MENU_BG, 1);
	} else {
		ILI9341_printText("           ", mess_val_xstart - 50,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				1);

		sprintf(str, "%.1f* ", u[adc_channel_pt100]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE,
		COLOR_MENU_BG, 2);
	}
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
/*
 void read_adc_raumtemp() {
 //running = true;
 draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
 COLOR_MENU_BAR_DOWN, "M-");
 draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
 COLOR_MENU_BAR_DOWN, "M+");
 //-----------------------------in2
 //ILI9341_setFont(&Font24);
 //-----------------------------in1
 read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
 //-----------------------------24V
 read_adc_channels_v(0, adc_val_xstart,
 adc_val_ystart + frame_val_ysize * 2);
 if (measuring_range_raumtemp == 1) {  //-50+50
 u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
 u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
 u[adc_channel_in1] = u[adc_channel_in1] * 10 - 50;
 sprintf(str, "%.1f*", u[adc_channel_in1]);
 ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
 COLOR_WHITE, COLOR_MENU_BG, 2);
 ILI9341_printText(" -50/50", mess_val_xstart,
 adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
 2);
 }
 if (measuring_range_raumtemp == 2) { //0-50
 u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
 u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
 u[adc_channel_in1] = u[adc_channel_in1] * 5;
 sprintf(str, "%.1f*", u[adc_channel_in1]);
 ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
 COLOR_WHITE, COLOR_MENU_BG, 2);
 ILI9341_printText("  0/50 ", mess_val_xstart,
 adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
 2);
 }
 if (measuring_range_raumtemp == 3) { //-15+35
 u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
 u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
 u[adc_channel_in1] = u[adc_channel_in1] * 5 - 15;
 sprintf(str, "%.1f*", u[adc_channel_in1]);
 ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
 COLOR_WHITE, COLOR_MENU_BG, 2);
 ILI9341_printText(" -15/35", mess_val_xstart,
 adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
 2);

 }
 //------------------------------Bat
 cnt_old_batt++;
 if (cnt_old_batt - cnt_new_batt >= 20) {
 cnt_old_batt = cnt_new_batt;
 read_adc_battery(x_pos_batt, y_pos_batt);
 }
 //running = false;
 }
 */
void read_adc_kanaltemp() {
	//running = true;
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M-");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M+");
	//-----------------------------in1
	read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------24V
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);

	if (measuring_range_kanaltemp == 1) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 10 - 50;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" -50/50", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);

	}
	if (measuring_range_kanaltemp == 2) { //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 10 - 20;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" -20/80", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_kanaltemp == 3) { //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 5 - 15;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" -15/35", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_kanaltemp == 4) { //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 13 - 10;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("-10/120", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_kanaltemp == 5) { //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 5;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/50  ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_kanaltemp == 6) { //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 10;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/100", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_kanaltemp == 7) { //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 16;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/160", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_kanaltemp == 8) { //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 25;
		sprintf(str, "%.1f* ", u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/250", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_feuchte() {
	//running = true;
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M-");
	//-----------------------------in2
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M+");
	//-----------------------------in1
	read_adc_channels_p(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------in2
	read_adc_channels_v(3, adc_val_xstart, adc_val_ystart + frame_val_ysize);
	//-----------------------------24V
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 3);

	if (measuring_range_feuchte == 1) {  //
		u[adc_channel_in2] = ((float) adc_filter_in2.filtered) * Vref / adcRes;
		u[adc_channel_in2] = u[adc_channel_in2] / (R2 / (R1 + R2));
		u[adc_channel_in2] = u[adc_channel_in2] * 10 - 20;
		sprintf(str, "%.1f* ", u[adc_channel_in2]);
		ILI9341_printText(str, adc_val_xstart,
				adc_val_ystart + frame_val_ysize * 2,
				COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("-20/80 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize * 2, COLOR_WHITE,
				COLOR_MENU_BG, 2);
	}
	if (measuring_range_feuchte == 2) { //
		u[adc_channel_in2] = ((float) adc_filter_in2.filtered) * Vref / adcRes;
		u[adc_channel_in2] = u[adc_channel_in2] / (R2 / (R1 + R2));
		u[adc_channel_in2] = u[adc_channel_in2] * 5;
		sprintf(str, "%.1f* ", u[adc_channel_in2]);
		ILI9341_printText(str, adc_val_xstart,
				adc_val_ystart + frame_val_ysize * 2,
				COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/50 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize * 2, COLOR_WHITE,
				COLOR_MENU_BG, 2);
	}
	if (measuring_range_feuchte == 3) { //
		u[adc_channel_in2] = ((float) adc_filter_in2.filtered) * Vref / adcRes;
		u[adc_channel_in2] = u[adc_channel_in2] / (R2 / (R1 + R2));
		u[adc_channel_in2] = u[adc_channel_in2] * 10 - 40;
		sprintf(str, "%.1f* ", u[adc_channel_in2]);
		ILI9341_printText(str, adc_val_xstart,
				adc_val_ystart + frame_val_ysize * 2,
				COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("-40/60 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize * 2, COLOR_WHITE,
				COLOR_MENU_BG, 2);
	}
	if (measuring_range_feuchte == 4) { //
		u[adc_channel_in2] = ((float) adc_filter_in2.filtered) * Vref / adcRes;
		u[adc_channel_in2] = u[adc_channel_in2] / (R2 / (R1 + R2));
		u[adc_channel_in2] = u[adc_channel_in2] * 5 - 15;
		sprintf(str, "%.1f* ", u[adc_channel_in2]);
		ILI9341_printText(str, adc_val_xstart,
				adc_val_ystart + frame_val_ysize * 2,
				COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("-15/35 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize * 2, COLOR_WHITE,
				COLOR_MENU_BG, 2);
	}
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_druckwasser() {
	//running = true;
	//draw_button(x_pos_button_l,y_pos_button_l,COLOR_WHITE,COLOR_MENU_BAR_DOWN,"M-");
	//draw_button(x_pos_button_r,y_pos_button_r,COLOR_WHITE,COLOR_MENU_BAR_DOWN,"M+");
//-----------------------------in2
	//-----------------------------in1
	read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------24V
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);

	ILI9341_printText("  0/6  ", mess_val_xstart,
			adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG, 2);

	u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
	u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
	u[adc_channel_in1] = u[adc_channel_in1] * 0.6;
	sprintf(str, "%.1fbar ", u[adc_channel_in1]);
	ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_druckluft() {
	//running = true;

	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M-");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M+");
	//-----------------------------in1
	read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------24V
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);

	if (measuring_range_druckluft == 1) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 2.5;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/25 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 2) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 5;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/50 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 3) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 10;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("  0/100", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 4) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 25;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/250 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 5) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 50;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/500 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 6) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 100;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/1000", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 7) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 150;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/1500", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 8) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 200;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/2000", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 9) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 250;
		sprintf(str, "%4dpa ", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/2500", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 10) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 5 - 25;
		sprintf(str, "%4dpa ", (int16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("-25/25 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 11) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 10 - 50;
		sprintf(str, "%4dpa ", (int16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("-50/50 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 12) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 20 - 100;
		sprintf(str, "%4dpa ", (int16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" -+100 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_druckluft == 13) {  //
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 30 - 150;
		sprintf(str, "%4dpa ", (int16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" -+150 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_luftquali() {
	//running = true;
	//-----------------------------in1
	read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------24V
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);
	//-----------------------------in1
	u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
	u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
	u[adc_channel_in1] = u[adc_channel_in1] * 200;
	sprintf(str, "%4dppm", (uint16_t) u[adc_channel_in1]);
	ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
	COLOR_WHITE, COLOR_MENU_BG, 2);

	ILI9341_printText(" 0/2000", mess_val_xstart,
			adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG, 2);
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_helligkeit() {
	//running = true;
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M-");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "M+");
	read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------24V
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);

	if (measuring_range_helligkeit == 1) {  //0/200
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 20;
		sprintf(str, "%5d", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/200 ", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_helligkeit == 2) { //0/1000
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 100;
		sprintf(str, "%5d", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/1000", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_helligkeit == 3) { //0/2k
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 200;
		sprintf(str, "%5d", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText(" 0/2000", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_helligkeit == 4) { //0/10k
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 1000;
		sprintf(str, "%5d", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("0/10000", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_helligkeit == 5) { //0/20k
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 2000;
		sprintf(str, "%5d", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("0/20000", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	if (measuring_range_helligkeit == 6) { //0/50k
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 5000;
		sprintf(str, "%5d", (uint16_t) u[adc_channel_in1]);
		ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		ILI9341_printText("0/50000", mess_val_xstart,
				adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG,
				2);
	}
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
}
void read_adc_pt100() {

	//running = true;

	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "min");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "std");
//-----------------------------pt100
	u[adc_channel_pt100] = (float) adc_filter_pt100.filtered;
	cnt_old_batt++;
	cnt_old_pt100++;
	//if (cnt_old1 - cnt_new1 >= 5) {
	//cnt_old1 = cnt_new1;
	u[adc_channel_pt100] = map(u[adc_channel_pt100], PT100_ADC_MIN,
	PT100_ADC_MAX, PT100_TEMP_MIN, PT100_TEMP_MAX) - PT100_ERROR; // new

	if (u[adc_channel_pt100] > 200 || u[adc_channel_pt100] < -20) {
		ILI9341_printText("fuehler fehlt", adc_val_xstart + 23,
				adc_val_ystart + frame_val_ysize * 2 + 20,
				COLOR_RED, COLOR_MENU_BG, 1);

	} else {
		ILI9341_printText("              ", adc_val_xstart + 23,
				adc_val_ystart + frame_val_ysize * 2 + 20,
				COLOR_WHITE, COLOR_MENU_BG, 1);

		sprintf(str, "%.1f* ", u[adc_channel_pt100]);
		ILI9341_printText(str, adc_val_xstart + 25,
				adc_val_ystart + frame_val_ysize * 3,
				COLOR_WHITE, COLOR_MENU_BG, 2.5);

		adc_pt100 = u[adc_channel_pt100];
		//}
		if (current_pt100_page == 1) {
			if (cnt_old_pt100 - cnt_new_pt100 >= 30) { //old interval_gr, 1 tick=0.5sec
				cnt_old_pt100 = cnt_new_pt100;
				cy_old += 5; //x pixel forward

				pt100_graphic_x_old = 25 + cy_old; //10
				pt100_graphic_y_old = 160 - adc_pt100; //10
				if (cy_old > 5) {

					for (uint8_t h = 0; h <= 2; h++)
						ILI9341_drawLine(pt100_graphic_x_new,
								pt100_graphic_y_new - h, pt100_graphic_x_old,
								pt100_graphic_y_old - h, COLOR_YELLOW);
				}
				//	ILI9341_drawPixel(pt100_graphic_x_old,pt100_graphic_y_old, COLOR_GREEN);
				pt100_graphic_x_new = pt100_graphic_x_old;
				pt100_graphic_y_new = pt100_graphic_y_old;

				//ILI9341_drawLine(25 + cy_old, 160 - adc_pt100, COLOR_GREEN);

				//ILI9341_drawPixel(25 + cy_old+j, 160 - adc_pt100+j, COLOR_GREEN);

				//ILI9341_fillCircle(25 + cy_old, 160 - adc_pt100, 2, COLOR_GREEN);
			}
			if (cy_old >= 200) {
				//uint16_t colorrand = rand();
				cy_old = 5;
			}
		}
		if (current_pt100_page == 2) {
			if (cnt_old_pt100 - cnt_new_pt100 >= 1440) { //1440 == 12 min
				cnt_old_pt100 = cnt_new_pt100;
				cy_old += 5;
				ILI9341_fillCircle(25 + cy_old, 160 - adc_pt100, 2,
				COLOR_GREEN);
			}
			if (cy_old >= 200) {
				cy_old = 5;
			}
		}
	}
	//------------------------------Bat
	cnt_old_batt++;
	if (cnt_old_batt - cnt_new_batt >= 20) {
		cnt_old_batt = cnt_new_batt;
		read_adc_battery(x_pos_batt, y_pos_batt);
	}
	//running = false;
	//-----------------------------Buzzer
}
//running = true;

void update_premenu_ysensor() {
	//running = true;
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, " -");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, " +");
	draw_button(x_pos_button_l, y_pos_button_l - 43, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "/10");
	draw_button(x_pos_button_r, y_pos_button_r - 43, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "*10");

	sprintf(str, "+%3d ", multiply_min_max);

	ILI9341_printText(str, frame_val_xstart + 15, frame_val_ystart + 5,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	ILI9341_printText(str, frame_val_xstart + 15,
			frame_val_ystart + frame_val_ysize * 2 + 5,
			COLOR_WHITE, COLOR_MENU_BG, 2);
	sprintf(str, "-%3d ", multiply_min_max);
	ILI9341_printText(str, frame_val_xstart - 155,
			frame_val_ystart + frame_val_ysize * 2 + 5,
			COLOR_WHITE, COLOR_MENU_BG, 2);
	ILI9341_printText(str, frame_val_xstart - 155, frame_val_ystart + 5,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	sprintf(str, "%4d", yvalue_min);
	ILI9341_printText(str, frame_val_xstart - 80, frame_val_ystart + 5,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	sprintf(str, "%4d", yvalue_max);
	ILI9341_printText(str, frame_val_xstart - 80,
			frame_val_ystart + frame_val_ysize * 2 + 5,
			COLOR_WHITE, COLOR_MENU_BG, 2);
//------------------------------Bat
	read_adc_battery(x_pos_batt, y_pos_batt);
	//running = false;
}
/*
 void messbereich_einstellen(char min, uint16_t max,
 uint16_t x, uint16_t y) {
 //-50  +50
 u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
 u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
 u[adc_channel_in1] = u[adc_channel_in1] * 10 - 50;
 sprintf(str, "%.1f*", u[adc_channel_in1]);
 ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
 COLOR_WHITE, COLOR_MENU_BG, 2);
 ILI9341_printText(" -50/50", mess_val_xstart,
 adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG, 2);
 }
 */
void read_adc_ysensor() {
	//running = true;
//-----------------------------in1
	read_adc_channels_v(2, adc_val_xstart, adc_val_ystart);
	//-----------------------------24V
	read_adc_channels_v(0, adc_val_xstart,
			adc_val_ystart + frame_val_ysize * 2);
	//-----------------------------in1
	u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
	u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
	u[adc_channel_in1] = map(u[adc_channel_in1], 0, 10, yvalue_min, yvalue_max);
	sprintf(str, "%.1f ", u[adc_channel_in1]);
	ILI9341_printText(str, adc_val_xstart, adc_val_ystart + frame_val_ysize,
	COLOR_WHITE, COLOR_MENU_BG, 2);

	sprintf(str, "%d/%d", yvalue_min, yvalue_max);
	ILI9341_printText(str, mess_val_xstart - 20,
			adc_val_ystart + frame_val_ysize, COLOR_WHITE, COLOR_MENU_BG, 2);

	//sprintf(str,"%d",yvalue_max);
	//ILI9341_string(220, 185, str, COLOR_WHITE, COLOR_MENU_BG);

	//------------------------------Bat
	read_adc_battery(x_pos_batt, y_pos_batt);
	//running = false;
}
void read_adc_channels_v(uint8_t channel, uint16_t x, uint16_t y) {
	switch (channel) {
	case 0: //24v
		u[adc_channel_24v] = adc_data[adc_channel_24v] * Vref / adcRes;
		//u[adc_channel_24v] = ((float) adc_filter_24v.filtered) * Vref / adcRes;
		u[adc_channel_24v] = u[adc_channel_24v] / (R3 / (R1 + R3));
		sprintf(str, "%.1fV ", u[adc_channel_24v]);
		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
		//	||HAL_GPIO_ReadPin (GPIOB, V24ON2_Pin)
//		cnt_old_fault++;
//		if (cnt_old_fault - cnt_new_fault >= 4) { // 2 sec
//			cnt_new_fault = cnt_old_fault;
//			cnt_new_fault = 0;
		if (u[adc_channel_24v] < ERROR_24V_MIN && status_idle == true) {
//		 			cnt_new_fault++;

			start_buzzer(alarm_ton);
			start_vibration();
//		 				cnt_new_fault = 0;
			//				HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_RESET);
		}

//	}
		else {
			start_buzzer(0);
		}

//||
		break;
	case 1:
		//y
		u[adc_channel_y] = ((float) adc_filter_y.filtered) * Vref / adcRes;
		u[adc_channel_y] = u[adc_channel_y] / (R2 / (R1 + R2));

		sprintf(str, "%.2fV ", u[adc_channel_y]);
		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
		if (u[adc_channel_y] > ERROR_Y_MAX) {
			start_buzzer(alarm_ton);
			start_vibration();
		} else {
			start_buzzer(0);
		}
		//set DAC
		//----
		u[adc_channel_y] = u[adc_channel_y] * 100; //in mV
		dac_time_old++;
		if (dac_time_old - dac_time_new >= 4) {
			dac_time_old = dac_time_new;

			sprintf(str, "%2dV ", DacVolt);
			ILI9341_printText(str, 20, 180,
			COLOR_WHITE, COLOR_MENU_BG, 2);

//		sprintf(str, "%3dDAC ", DacVal);
//		ILI9341_printText(str, 80, 180,
//		COLOR_WHITE, COLOR_MENU_BG, 2);
			switch (DacVolt) {
			case 0:
				DacVal = 0;	 //
				break;
			case 1:
				DacVal = DAC1V;
				break;
			case 2:
				DacVal = DAC2V;
				break;
			case 3:
				DacVal = DAC3V;
				break;
			case 4:
				DacVal = DAC4V;
				break;
			case 5:
				DacVal = DAC5V;
				break;
			case 6:
				DacVal = DAC6V;
				break;
			case 7:
				DacVal = DAC7V;
				break;
			case 8:
				DacVal = DAC8V;
				break;
			case 9:
				DacVal = DAC9V;
				break;
			case 10:
				DacVal = DAC10V;
				break;
			case 11:
				DacVal = DAC11V;
				break;
			case 12:
				DacVal = DAC12V;
				break;
			default:
				DacVal = 0;
				break;
			}
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DacVal); //DacVal
			//u[adc_channel_y] = (uint16_t) u[adc_channel_y] * 100; //  in mV Y
//		if ((uint16_t)u[adc_channel_y] <= DacVolt*100 || (uint16_t)u[adc_channel_y] <= DacVolt*100 - 200) {
//			DacVal += 50;
//			//HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DacVal);
//			//value_matched = true;
//		}
//		if (DacVal > 4000) {
//			DacVal = 4000;
//		}
//		if ((uint16_t)u[adc_channel_y] >= DacVolt*100 || (uint16_t)u[adc_channel_y] >= DacVolt*100  + 200) {
//			DacVal -= 50;
//		}
//		if (DacVal <= 0) {
//			DacVal = 0;
//		}
//		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DacVal);
//		//----
		}
		break;
	case 2:
		//in1
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		sprintf(str, "%.2fV ", u[adc_channel_in1]);
		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
		if (u[adc_channel_in1] > ERROR_IN1_MAX) {
			start_buzzer(alarm_ton);
			start_vibration();
		} else {
			start_buzzer(0);
		}
		break;
	case 3:
		//in2
		u[adc_channel_in2] = ((float) adc_filter_in2.filtered) * Vref / adcRes;
		u[adc_channel_in2] = u[adc_channel_in2] / (R2 / (R1 + R2));
		sprintf(str, "%.2fV ", u[adc_channel_in2]);
		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
		if (u[adc_channel_in2] > ERROR_IN2_MAX) {
			start_buzzer(alarm_ton);
			start_vibration();
		} else {
			start_buzzer(0);
		}
		break;
	}

}
void read_adc_channels_p(uint8_t channel, uint16_t x, uint16_t y) {
	switch (channel) {
	case 0: //24v
//		u[adc_channel_24v] = ((float) adc_filter_24v.filtered) * Vref / adcRes;
//		u[adc_channel_24v] = u[adc_channel_24v] / (R3 / (R1 + R3));
//		sprintf(str, "%3d", (uint8_t)u[adc_channel_24v]);
//		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
//		if (u[adc_channel_24v] < 12) {
//			start_buzzer(alarm_ton);
//			start_vibration();
//		} else {
//			start_buzzer(0);
//		}
		break;
	case 1: //y
		u[adc_channel_y] = ((float) adc_filter_y.filtered) * Vref / adcRes;
		u[adc_channel_y] = u[adc_channel_y] / (R2 / (R1 + R2));
		u[adc_channel_y] = u[adc_channel_y] * 10;
		sprintf(str, "%3d%% ", (uint8_t) u[adc_channel_y]);
		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
		if (u[adc_channel_y] > ERROR_Y_MAX * 10) {
			start_buzzer(alarm_ton);
			start_vibration();
		} else {
			start_buzzer(0);
		}
		break;
	case 2: //in1
		u[adc_channel_in1] = ((float) adc_filter_in1.filtered) * Vref / adcRes;
		u[adc_channel_in1] = u[adc_channel_in1] / (R2 / (R1 + R2));
		u[adc_channel_in1] = u[adc_channel_in1] * 10;
		sprintf(str, "%3d%% ", (uint8_t) u[adc_channel_in1]);
		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
		if (u[adc_channel_in1] > ERROR_IN1_MAX * 10) {
			start_buzzer(alarm_ton);
			start_vibration();
		} else {
			start_buzzer(0);
		}
		break;
	case 3: //in2
		u[adc_channel_in2] = ((float) adc_filter_in2.filtered) * Vref / adcRes;
		u[adc_channel_in2] = u[adc_channel_in2] / (R2 / (R1 + R2));
		u[adc_channel_in2] = u[adc_channel_in2] * 10;
		sprintf(str, "%3d%% ", (uint8_t) u[adc_channel_in2]);
		ILI9341_printText(str, x, y, COLOR_WHITE, COLOR_MENU_BG, 2);
		if (u[adc_channel_in2] > ERROR_IN2_MAX * 10) {
			start_buzzer(alarm_ton);
			start_vibration();
		} else {
			start_buzzer(0);
		}
		break;
	}

}
void adc_menu_update() {
//read_adc_allchannels();
	running = true;
	switch (current_main_page) {
	case 0: //
		break;
	case 1: //
		read_adc_home();
		break;
	case 2: //
		read_adc_info();
		break;
	case 3: //
		update_tools_menu();
		break;
	}
	switch (current_sensor_page) {
	case 0: //
		break;
	case 1: //
		read_adc_raumregler_aktiv();
		break;
	case 2: //
		read_adc_raumregler_passiv();
		break;
//case 3: //
//	read_adc_raumtemp();
//	break;
	case 3: //
		read_adc_kanaltemp();
		break;
	case 4: //
		read_adc_feuchte();
		break;
	case 5: //
		read_adc_druckwasser();
		break;
	case 6: //
		read_adc_druckluft();
		break;
	case 7: //
		read_adc_luftquali();
		break;
	case 8: //
		read_adc_helligkeit();
		break;
	case 9: //
		read_adc_pt100();
		break;
	case 10: //
		update_premenu_ysensor();
		break;
	case 13: //
		read_adc_ysensor();
		break;
	default:
		break;
	}
	switch (current_antrieb_page) {
	case 0: //
		break;
	case 1: //
		read_adc_antrieb_r();
		break;
	case 2: //
		read_adc_antrieb_s();
		break;
	default:
		break;
	}
	running = false;
}
void ini_adc_filter() {
	uint8_t filter_size_high = 50;
	uint8_t filter_size_medium = 40;
	uint8_t filter_size_low = 40;

	uint16_t filter_time_high = 100;
	uint16_t filter_time_medium = 100;
	uint16_t filter_time_low = 100;

	moving_median_create(&adc_filter_pt100, filter_size_high, filter_time_high);
//moving_median_create(&adc_filter_24v, filter_size_low, filter_time_low);
	moving_median_create(&adc_filter_y, filter_size_high, filter_time_high);
	moving_median_create(&adc_filter_in1, filter_size_high, filter_time_high);
	moving_median_create(&adc_filter_in2, filter_size_high, filter_time_high);
	moving_median_create(&adc_filter_temp, filter_size_medium,
			filter_time_medium);
	moving_median_create(&adc_filter_bat, filter_size_low, filter_time_low);
}
void adc_filter_loop() {

	moving_median_filter(&adc_filter_pt100, adc_data[adc_channel_pt100]);
////moving_median_filter(&adc_filter_24v, adc_data[adc_channel_24v]);
	moving_median_filter(&adc_filter_y, adc_data[adc_channel_y]);
	moving_median_filter(&adc_filter_in1, adc_data[adc_channel_in1]);
	moving_median_filter(&adc_filter_in2, adc_data[adc_channel_in2]);
	moving_median_filter(&adc_filter_temp, adc_data[adc_channel_temp]);
	moving_median_filter(&adc_filter_bat, adc_data[adc_channel_bat]);
//moving_median_filter(&med_filter6, adc_data[6]);
}
