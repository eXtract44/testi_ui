/*
 * adc.h
 *
 *  Created on: 22.07.2021
 *      Author: Alex
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include  <stdint.h>

#define NUMBER_OF_CHANNELS 6
#define R1 100000.0f//for 10V lines 100k
#define R2 26700.0f //for 10V lines 27k
#define Vref 3.3f
#define adcRes 4095.0f


enum dac_output_voltage {
	DAC0V = 0, //ok
	DAC1V = 340, //ok
	DAC2V = 705, //ok
	DAC3V = 1040, //ok
	DAC4V = 1370, //ok
	DAC5V = 1700, //ok
	DAC6V = 2033, //ok
	DAC7V = 2364, //ok
	DAC8V = 2695, //ok
	DAC9V = 3024, //ok
	DAC10V = 3355, //ok
	DAC11V = 3690, //ok
	DAC12V = 4095 //ok
};


#define POTENTIOMETR_ADC_MIN 2612//
#define POTENTIOMETR_ADC_MAX 2594//

#define PT100_ADC_MIN 227//
#define PT100_ADC_MAX 3162//
#define PT100_TEMP_MIN -20//
#define PT100_TEMP_MAX 200//
#define PT100_ERROR 1

// Datasheet
//Overcharge Detection Voltage: 4.25-4.35V±0.05V
//Over Discharge Detection Voltage: 2.3-3.0V±0.05V
///
#define BATTERY_FULL_VOLTAGE 8.405f
#define BATTERY_LOW_VOLTAGE 6.0f

#define UNDERVOLTAGE_OUTPUT_24V 20000 //mV
#define OVERVOLTAGE_INPUT_Y 12
#define OVERVOLTAGE_INPUT_IN1 12
#define OVERVOLTAGE_INPUT_IN2 12
#define INPUT_ACTIVATE 5


enum adc_channel{
ADC_CHANNEL_BATTERY ,  //PA2
ADC_CHANNEL_PT100 ,  //PA3
ADC_CHANNEL_Y , //PC1
ADC_CHANNEL_IN1 ,//PC2
ADC_CHANNEL_IN2 ,//PC3
ADC_CHANNEL_MCU_TEMPERATURE
};

int8_t read_adc_mcuTemp();
float read_adc_battery();
float read_adc_y();
float read_adc_in1();
float read_adc_in2();
float read_adc_pt100();
void set_y(uint8_t volt);
void ini_adc_filter();
void adc_filter_loop();
uint16_t read_adc_in1_potentiometer();
//void adc_menu_update();
#endif /* INC_ADC_H_ */
