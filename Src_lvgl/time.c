
#include "stm32f4xx_hal.h"
#include "time.h"
#include "spi_ili9341.h"
#include  <stdint.h>

//RTC_HandleTypeDef hrtc;
//RTC_DateTypeDef GetData;  //Get date structure
//RTC_TimeTypeDef GetTime;   //Get time structure

//char time[10];
////char date[10];
//
//uint16_t get_time()
//{
// RTC_DateTypeDef gDate;
// RTC_TimeTypeDef gTime;
///* Get the RTC current Time */
// HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
///* Get the RTC current Date */
// HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
///* Display time Format: hh:mm:ss */
// sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);
// return time[];
// //ILI9341_printText(time, 135, 17, COLOR_WHITE, COLOR_MENU_BAR_DOWN, 1);
///* Display date Format: dd-mm-yy */
// //sprintf((char*)date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);
// //ILI9341_printText(date, 30, 45, COLOR_WHITE, COLOR_MENU_BG, 1);
//}



