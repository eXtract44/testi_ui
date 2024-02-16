/*
 * time.c
 *
 *  Created on: 22.02.2024
 *      Author: Oleksandr Onopriienko
 */
#include "main.h"
#include "time.h"
#include "spi_ili9341.h"
#include "menu.h"
#include  <stdint.h>

#include "stm32f4xx_hal.h"
RTC_HandleTypeDef hrtc;

char string_buffer_time[10];
print_time(135, 17,COLOR_WHITE,COLOR_MENU_BAR_DOWN,1,true);

void print_time(int16_t x, int16_t y,uint16_t time_color,uint16_t time_bg,uint8_t time_size,bool seconds_active){
get_time();
if(seconds_active == true){
 sprintf((char*)string_buffer_time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);
}else{
  sprintf((char*)string_buffer_time,"%02d:%02d",gTime.Hours, gTime.Minutes);  
}
 ILI9341_printText(string_buffer_time, x, y, time_color, time_bg, time_size);
}
void get_time(){
 RTC_DateTypeDef gDate;
 RTC_TimeTypeDef gTime;
/* Get the RTC current Time */
 HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
/* Get the RTC current Date */
 HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
/* Display time Format: hh:mm:ss */
}



