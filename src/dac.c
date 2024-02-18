/*
 * adc.c
 *
 *  Created on: 22.02.2024
 *      Author: Oleksandr Onopriienko
 */
#include "dac.h"




extern DAC_HandleTypeDef hdac;

void set_dac_to_volt(uint8_t volt){
    if(volt> DAC_MAX_VOLTAGE){
        volt = DAC_MAX_VOLTAGE;
    }
  switch (volt){
case 0:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_0V);
break; 
case 1:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_1V);
break; 
case 2:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_2V);
break; 
case 3:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_3V);
break; 
case 4:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_4V);
break; 
case 5:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_5V);
break; 
case 6:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_6V);
break; 
case 7:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_7V);
break; 
case 8:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_8V);
break; 
case 9:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_9V);
break; 
case 10:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_10V);
break; 
case 11:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_11V);
break; 
case 12:
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DAC_12V);
break; 
  }  
}