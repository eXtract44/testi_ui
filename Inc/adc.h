/*
 * adc.h
 *
 *  Created on: 22.07.2021
 *      Author: Alex
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32f4xx_hal.h"
//start Alex
#define R1 100000.0f//for 10/24V lines 100k
#define R2 26560.0f //for 10V lines 	27k
#define R3 14200.0f //for 24V lines 	15k
#define Vref 3.336f

#define DAC1V 370//ok
#define DAC2V 705//ok
#define DAC3V 1040//ok
#define DAC4V 1370//ok
#define DAC5V 1700//ok
#define DAC6V 2033//ok
#define DAC7V 2364//ok
#define DAC8V 2695//ok
#define DAC9V 3024//ok
#define DAC10V 3355//ok
#define DAC11V 3690//ok
#define DAC12V 4095 //ok

#define POTI_ADC_MIN 642//
#define POTI_ADC_MAX 638//

#define PT100_ADC_MIN 62//
#define PT100_ADC_MAX 787//
#define PT100_TEMP_MIN -20//
#define PT100_TEMP_MAX 200//
#define PT100_ERROR 1


//end Alex


#define adcRes 1023.0f

#define ERROR_24V_MIN 20
#define ERROR_Y_MAX 12
#define ERROR_IN1_MAX 12
#define ERROR_IN2_MAX 12

#define adc_channel_pt100 0
#define adc_channel_24v 1
#define adc_channel_y 2
#define adc_channel_in1 3
#define adc_channel_in2 4
#define adc_channel_temp 5
#define adc_channel_bat 6

void read_adc_battery(uint16_t x, uint16_t y);
void read_adc_home();
void read_adc_info();
void read_adc_antrieb_r();
void read_adc_antrieb_s();
void read_adc_raumregler_aktiv();
void read_adc_raumregler_passiv();
void read_adc_raumtemp();
void read_adc_kanaltemp();
void read_adc_feuchte();
void read_adc_druckwasser();
void read_adc_druckluft();
void read_adc_luftquali();
void read_adc_helligkeit();
void read_adc_pt100();
void update_premenu_ysensor();
void read_adc_ysensor();
void read_adc_channels_v(uint8_t channel, uint16_t x, uint16_t y);
void read_adc_channels_p(uint8_t channel, uint16_t x, uint16_t y);
void adc_filter();
void read_adc_allchannels();
void ini_adc_filter();
void adc_filter_loop();
void adc_menu_update();
#endif /* INC_ADC_H_ */
