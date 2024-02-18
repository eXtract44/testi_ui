#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32f4xx_hal.h"

#define R1 100000.0f//for 10/24V lines 100k
#define R2 26560.0f //for 10V lines 	27k
#define R3 14200.0f //for 24V lines 	15k
#define Vref 3.3f
#define adcRes 1023.0f

#define ADC_CHANNEL_QUANTITY 7


enum alarm_values{	
ALARM_VALUE_OUTPUT_24V=20,
ALARM_VALUE_OUTPUT_0_10V=12,
ALARM_VALUE_INPUT_1_2_0_10V=12
};
enum adc_names{
ADC_CHANNEL_OUTPUT_24V,
ADC_CHANNEL_OUTPUT_0_10V,
ADC_CHANNEL_INPUT1_0_10V,
ADC_CHANNEL_INPUT2_0_10V,
ADC_CHANNEL_INPUT_PT100,
ADC_CHANNEL_MCU_TEMPERATURE,
ADC_CHANNEL_BATTERY,
};


#define PT100_ADC_MIN 62//
#define PT100_ADC_MAX 787//
#define PT100_TEMP_MIN -20//
#define PT100_TEMP_MAX 200//
#define PT100_ERROR 1







#endif /* INC_ADC_H_ */
