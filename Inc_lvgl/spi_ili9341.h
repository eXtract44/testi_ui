#ifndef __SPI_ILI9341_H
#define __SPI_ILI9341_H
//-------------------------------------------------------------------
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

//-------------------------------------------------------------------
#define RESET_ACTIVE() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET)
#define RESET_IDLE() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET)
#define CS_ACTIVE() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)
#define CS_IDLE() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
#define DC_COMMAND() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET)
#define DC_DATA() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET)
//-------------------------------------------------------------------
#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)
#define ILI9341_VSCRDEF 0x33


//-------------------------------------------------------------------
//***** Functions prototypes *****//
void ILI9341_SendCommand(uint8_t com);
void ILI9341_SendData(uint8_t data);
void ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2,
		uint16_t y2);
void ILI9341_SetBrightness(uint8_t brightness);
void ILI9341_Delay(uint32_t dly);
void DelaySeconds(uint32_t __IO seconds);
void ILI9341_ini(uint16_t w_size, uint16_t h_size);
void ILI9341_setRotation(uint8_t rotate);
void ILI9341_SetRotation(uint8_t r);
void ILI9341_Set_Address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
//-------------------------------------------------------------------
#endif /* __SPI_ILI9341_H */
