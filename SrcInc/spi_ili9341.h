#ifndef __SPI_ILI9341_H
#define __SPI_ILI9341_H
//-------------------------------------------------------------------
//#include "stm32f4xx_hal.h"
//#include "stm32f4xx_hal_tim.h"
#include <stdlib.h>
#include "fonts.h"
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


//#define ILI9341_HEIGHT 240
//#define ILI9341_WIDTH 320
#define BURST_MAX_SIZE 	500
#define ILI9341_PIXEL_COUNT 320 * 240
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define min(a,b) (((a)<(b))?(a):(b))
//***Colors

//List of colors
#define COLOR_BLACK           0x0000
#define COLOR_NAVY            0x000F
#define COLOR_DGREEN          0x03E0
#define COLOR_DCYAN           0x03EF
#define COLOR_MAROON          0x7800
#define COLOR_PURPLE          0x780F
#define COLOR_OLIVE           0x7BE0
#define COLOR_LGRAY           0xC618
#define COLOR_DGRAY           0x7BEF
#define COLOR_BLUE            0x001F
#define COLOR_BLUE2			  0x051D
#define COLOR_GREEN           0x07E0
#define COLOR_GREEN2		  0xB723
#define COLOR_GREEN3		  0x8000
#define COLOR_CYAN            0x07FF
#define COLOR_RED             0xF800
#define COLOR_MAGENTA         0xF81F
#define COLOR_YELLOW          0xFFE0
#define COLOR_WHITE           0xFFFF
#define COLOR_ORANGE          0xFD20
#define COLOR_GREENYELLOW     0xAFE5
#define COLOR_BROWN 		  0XBC40




//Text simple font array (You can your own font)
static const unsigned char font1[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x5B,
		0x4F, 0x5B, 0x3E, 0x3E, 0x6B, 0x4F, 0x6B, 0x3E, 0x1C, 0x3E, 0x7C, 0x3E,
		0x1C, 0x18, 0x3C, 0x7E, 0x3C, 0x18, 0x1C, 0x57, 0x7D, 0x57, 0x1C, 0x1C,
		0x5E, 0x7F, 0x5E, 0x1C, 0x00, 0x18, 0x3C, 0x18, 0x00, 0xFF, 0xE7, 0xC3,
		0xE7, 0xFF, 0x00, 0x18, 0x24, 0x18, 0x00, 0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
		0x30, 0x48, 0x3A, 0x06, 0x0E, 0x26, 0x29, 0x79, 0x29, 0x26, 0x40, 0x7F,
		0x05, 0x05, 0x07, 0x40, 0x7F, 0x05, 0x25, 0x3F, 0x5A, 0x3C, 0xE7, 0x3C,
		0x5A, 0x7F, 0x3E, 0x1C, 0x1C, 0x08, 0x08, 0x1C, 0x1C, 0x3E, 0x7F, 0x14,
		0x22, 0x7F, 0x22, 0x14, 0x5F, 0x5F, 0x00, 0x5F, 0x5F, 0x06, 0x09, 0x7F,
		0x01, 0x7F, 0x00, 0x66, 0x89, 0x95, 0x6A, 0x60, 0x60, 0x60, 0x60, 0x60,
		0x94, 0xA2, 0xFF, 0xA2, 0x94, 0x08, 0x04, 0x7E, 0x04, 0x08, 0x10, 0x20,
		0x7E, 0x20, 0x10, 0x08, 0x08, 0x2A, 0x1C, 0x08, 0x08, 0x1C, 0x2A, 0x08,
		0x08, 0x1E, 0x10, 0x10, 0x10, 0x10, 0x0C, 0x1E, 0x0C, 0x1E, 0x0C, 0x30,
		0x38, 0x3E, 0x38, 0x30, 0x06, 0x0E, 0x3E, 0x0E, 0x06, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
		0x14, 0x7F, 0x14, 0x7F, 0x14, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13,
		0x08, 0x64, 0x62, 0x36, 0x49, 0x56, 0x20, 0x50, 0x00, 0x08, 0x07, 0x03,
		0x00, 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x2A,
		0x1C, 0x7F, 0x1C, 0x2A, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x80, 0x70,
		0x30, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x60, 0x60, 0x00,
		0x20, 0x10, 0x08, 0x04, 0x02, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x42,
		0x7F, 0x40, 0x00, 0x72, 0x49, 0x49, 0x49, 0x46, 0x21, 0x41, 0x49, 0x4D,
		0x33, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x27, 0x45, 0x45, 0x45, 0x39, 0x3C,
		0x4A, 0x49, 0x49, 0x31, 0x41, 0x21, 0x11, 0x09, 0x07, 0x36, 0x49, 0x49,
		0x49, 0x36, 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00, 0x14, 0x00, 0x00,
		0x00, 0x40, 0x34, 0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x41, 0x14, 0x14,
		0x14, 0x14, 0x14, 0x00, 0x41, 0x22, 0x14, 0x08, 0x02, 0x01, 0x59, 0x09,
		0x06, 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x7F,
		0x49, 0x49, 0x49, 0x36, 0x3E, 0x41, 0x41, 0x41, 0x22, 0x7F, 0x41, 0x41,
		0x41, 0x3E, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x7F, 0x09, 0x09, 0x09, 0x01,
		0x3E, 0x41, 0x41, 0x51, 0x73, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x41,
		0x7F, 0x41, 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22,
		0x41, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x7F,
		0x04, 0x08, 0x10, 0x7F, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x7F, 0x09, 0x09,
		0x09, 0x06, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x7F, 0x09, 0x19, 0x29, 0x46,
		0x26, 0x49, 0x49, 0x49, 0x32, 0x03, 0x01, 0x7F, 0x01, 0x03, 0x3F, 0x40,
		0x40, 0x40, 0x3F, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x3F, 0x40, 0x38, 0x40,
		0x3F, 0x63, 0x14, 0x08, 0x14, 0x63, 0x03, 0x04, 0x78, 0x04, 0x03, 0x61,
		0x59, 0x49, 0x4D, 0x43, 0x00, 0x7F, 0x41, 0x41, 0x41, 0x02, 0x04, 0x08,
		0x10, 0x20, 0x00, 0x41, 0x41, 0x41, 0x7F, 0x04, 0x02, 0x01, 0x02, 0x04,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x03, 0x07, 0x08, 0x00, 0x20, 0x54,
		0x54, 0x78, 0x40, 0x7F, 0x28, 0x44, 0x44, 0x38, 0x38, 0x44, 0x44, 0x44,
		0x28, 0x38, 0x44, 0x44, 0x28, 0x7F, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00,
		0x08, 0x7E, 0x09, 0x02, 0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x7F, 0x08, 0x04,
		0x04, 0x78, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x20, 0x40, 0x40, 0x3D, 0x00,
		0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, 0x7C, 0x04,
		0x78, 0x04, 0x78, 0x7C, 0x08, 0x04, 0x04, 0x78, 0x38, 0x44, 0x44, 0x44,
		0x38, 0xFC, 0x18, 0x24, 0x24, 0x18, 0x18, 0x24, 0x24, 0x18, 0xFC, 0x7C,
		0x08, 0x04, 0x04, 0x08, 0x48, 0x54, 0x54, 0x54, 0x24, 0x04, 0x04, 0x3F,
		0x44, 0x24, 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x1C, 0x20, 0x40, 0x20, 0x1C,
		0x3C, 0x40, 0x30, 0x40, 0x3C, 0x44, 0x28, 0x10, 0x28, 0x44, 0x4C, 0x90,
		0x90, 0x90, 0x7C, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x08, 0x36, 0x41,
		0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x41, 0x36, 0x08, 0x00, 0x02,
		0x01, 0x02, 0x04, 0x02, 0x3C, 0x26, 0x23, 0x26, 0x3C, 0x1E, 0xA1, 0xA1,
		0x61, 0x12, 0x3A, 0x40, 0x40, 0x20, 0x7A, 0x38, 0x54, 0x54, 0x55, 0x59,
		0x21, 0x55, 0x55, 0x79, 0x41, 0x22, 0x54, 0x54, 0x78, 0x42, 0x21, 0x55,
		0x54, 0x78, 0x40, 0x20, 0x54, 0x55, 0x79, 0x40, 0x0C, 0x1E, 0x52, 0x72,
		0x12, 0x39, 0x55, 0x55, 0x55, 0x59, 0x39, 0x54, 0x54, 0x54, 0x59, 0x39,
		0x55, 0x54, 0x54, 0x58, 0x00, 0x00, 0x45, 0x7C, 0x41, 0x00, 0x02, 0x45,
		0x7D, 0x42, 0x00, 0x01, 0x45, 0x7C, 0x40, 0x7D, 0x12, 0x11, 0x12, 0x7D,
		0xF0, 0x28, 0x25, 0x28, 0xF0, 0x7C, 0x54, 0x55, 0x45, 0x00, 0x20, 0x54,
		0x54, 0x7C, 0x54, 0x7C, 0x0A, 0x09, 0x7F, 0x49, 0x32, 0x49, 0x49, 0x49,
		0x32, 0x3A, 0x44, 0x44, 0x44, 0x3A, 0x32, 0x4A, 0x48, 0x48, 0x30, 0x3A,
		0x41, 0x41, 0x21, 0x7A, 0x3A, 0x42, 0x40, 0x20, 0x78, 0x00, 0x9D, 0xA0,
		0xA0, 0x7D, 0x3D, 0x42, 0x42, 0x42, 0x3D, 0x3D, 0x40, 0x40, 0x40, 0x3D,
		0x3C, 0x24, 0xFF, 0x24, 0x24, 0x48, 0x7E, 0x49, 0x43, 0x66, 0x2B, 0x2F,
		0xFC, 0x2F, 0x2B, 0xFF, 0x09, 0x29, 0xF6, 0x20, 0xC0, 0x88, 0x7E, 0x09,
		0x03, 0x20, 0x54, 0x54, 0x79, 0x41, 0x00, 0x00, 0x44, 0x7D, 0x41, 0x30,
		0x48, 0x48, 0x4A, 0x32, 0x38, 0x40, 0x40, 0x22, 0x7A, 0x00, 0x7A, 0x0A,
		0x0A, 0x72, 0x7D, 0x0D, 0x19, 0x31, 0x7D, 0x26, 0x29, 0x29, 0x2F, 0x28,
		0x26, 0x29, 0x29, 0x29, 0x26, 0x30, 0x48, 0x4D, 0x40, 0x20, 0x38, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x2F, 0x10, 0xC8, 0xAC,
		0xBA, 0x2F, 0x10, 0x28, 0x34, 0xFA, 0x00, 0x00, 0x7B, 0x00, 0x00, 0x08,
		0x14, 0x2A, 0x14, 0x22, 0x22, 0x14, 0x2A, 0x14, 0x08, 0x55, 0x00, 0x55,
		0x00, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0xFF, 0x55, 0xFF, 0x55, 0xFF,
		0x00, 0x00, 0x00, 0xFF, 0x00, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x14, 0x14,
		0x14, 0xFF, 0x00, 0x10, 0x10, 0xFF, 0x00, 0xFF, 0x10, 0x10, 0xF0, 0x10,
		0xF0, 0x14, 0x14, 0x14, 0xFC, 0x00, 0x14, 0x14, 0xF7, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x14, 0x14, 0xF4, 0x04, 0xFC, 0x14, 0x14, 0x17,
		0x10, 0x1F, 0x10, 0x10, 0x1F, 0x10, 0x1F, 0x14, 0x14, 0x14, 0x1F, 0x00,
		0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10,
		0x10, 0x1F, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x10, 0x00, 0x00, 0x00, 0xFF,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x10, 0x00,
		0x00, 0x00, 0xFF, 0x14, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x1F,
		0x10, 0x17, 0x00, 0x00, 0xFC, 0x04, 0xF4, 0x14, 0x14, 0x17, 0x10, 0x17,
		0x14, 0x14, 0xF4, 0x04, 0xF4, 0x00, 0x00, 0xFF, 0x00, 0xF7, 0x14, 0x14,
		0x14, 0x14, 0x14, 0x14, 0x14, 0xF7, 0x00, 0xF7, 0x14, 0x14, 0x14, 0x17,
		0x14, 0x10, 0x10, 0x1F, 0x10, 0x1F, 0x14, 0x14, 0x14, 0xF4, 0x14, 0x10,
		0x10, 0xF0, 0x10, 0xF0, 0x00, 0x00, 0x1F, 0x10, 0x1F, 0x00, 0x00, 0x00,
		0x1F, 0x14, 0x00, 0x00, 0x00, 0xFC, 0x14, 0x00, 0x00, 0xF0, 0x10, 0xF0,
		0x10, 0x10, 0xFF, 0x10, 0xFF, 0x14, 0x14, 0x14, 0xFF, 0x14, 0x10, 0x10,
		0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x38, 0x44, 0x44,
		0x38, 0x44, 0xFC, 0x4A, 0x4A, 0x4A, 0x34, 0x7E, 0x02, 0x02, 0x06, 0x06,
		0x02, 0x7E, 0x02, 0x7E, 0x02, 0x63, 0x55, 0x49, 0x41, 0x63, 0x38, 0x44,
		0x44, 0x3C, 0x04, 0x40, 0x7E, 0x20, 0x1E, 0x20, 0x06, 0x02, 0x7E, 0x02,
		0x02, 0x99, 0xA5, 0xE7, 0xA5, 0x99, 0x1C, 0x2A, 0x49, 0x2A, 0x1C, 0x4C,
		0x72, 0x01, 0x72, 0x4C, 0x30, 0x4A, 0x4D, 0x4D, 0x30, 0x30, 0x48, 0x78,
		0x48, 0x30, 0xBC, 0x62, 0x5A, 0x46, 0x3D, 0x3E, 0x49, 0x49, 0x49, 0x00,
		0x7E, 0x01, 0x01, 0x01, 0x7E, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x44, 0x44,
		0x5F, 0x44, 0x44, 0x40, 0x51, 0x4A, 0x44, 0x40, 0x40, 0x44, 0x4A, 0x51,
		0x40, 0x00, 0x00, 0xFF, 0x01, 0x03, 0xE0, 0x80, 0xFF, 0x00, 0x00, 0x08,
		0x08, 0x6B, 0x6B, 0x08, 0x36, 0x12, 0x36, 0x24, 0x36, 0x06, 0x0F, 0x09,
		0x0F, 0x06, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00,
		0x30, 0x40, 0xFF, 0x01, 0x01, 0x00, 0x1F, 0x01, 0x01, 0x1E, 0x00, 0x19,
		0x1D, 0x17, 0x12, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00,
		0x00 };

//-------------------------------------------------------------------
#define swap(a,b) {int16_t t=a;a=b;b=t;}
//-------------------------------------------------------------------
//***** Functions prototypes *****//
//1. Write Command to LCD
void ILI9341_SendCommand(uint8_t com);
//2. Write data to LCD
void ILI9341_SendData(uint8_t data);
//3. Set cursor position
void ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2,
		uint16_t y2);
void ILI9341_SetBrightness(uint8_t brightness);
//3.1 delays
void ILI9341_Delay(uint32_t dly);
void DelaySeconds(uint32_t __IO seconds);
void DelayMili(uint32_t __IO mili);
//4. Initialise function
void ILI9341_ini(uint16_t w_size, uint16_t h_size);
//4. Initialise fonts
void ILI9341_fontsIni(void);
void ILI9341_setFont(sFONT *pFonts);
//5. Write data to a single pixel
void ILI9341_drawPixel(uint16_t x, uint16_t y, uint16_t color); //Draw single pixel to ILI9341
//6. Fill the entire screen with a background color
void ILI9341_fill(uint16_t color); //Fill entire ILI9341 with color
//7. Rectangle drawing functions
void ILI9341_fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
		uint16_t color);
void ILI9341_drawRect(unsigned int x0, unsigned int y0, unsigned int x1,
		unsigned int y1, uint16_t color);
void ILI9341_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
		int16_t r, uint16_t color);
void ILI9341_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
		int16_t r, uint16_t color);
//8. Circle drawing functions
void ILI9341_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
//static void drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
//		uint8_t cornername, uint16_t color);
void ILI9341_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername, int16_t delta, uint16_t color);
//9. Line drawing functions
void ILI9341_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		uint16_t color);
void ILI9341_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void ILI9341_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
//10. Triangle drawing
void ILI9341_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		int16_t x2, int16_t y2, uint16_t color);
void ILI9341_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		int16_t x2, int16_t y2, uint16_t color);
//11. Text printing functions
void ILI9341_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
		uint16_t bg, uint8_t size);
void ILI9341_drawCharF(uint16_t x, uint16_t y, uint8_t c, uint16_t color,
		uint16_t bg);
void ILI9341_string(uint16_t x, uint16_t y, char *str, uint16_t color,
		uint16_t bg);
void ILI9341_printText(char text[], int16_t x, int16_t y, uint16_t color,
		uint16_t bg, uint8_t size);
//12. Image print (RGB 565, 2 bytes per pixel)
void ILI9341_printImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		uint8_t *data, uint32_t size);
// Image print (Mono )
void ILI9341_drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,
		uint16_t color);
void ILI9341_drawBitmapFast(int16_t x, int16_t y,const uint8_t *bitmap, int16_t w,
		int16_t h, uint16_t color, uint32_t size);
void ILI9341_drawBitmapFastBar(int16_t x, int16_t y, const uint8_t *bitmap,
		int16_t w, int16_t h, uint16_t color, uint32_t size);
//13. Set screen rotation
void ILI9341_setRotation(uint8_t rotate);
uint16_t ILI9341_RandColor();


void ILI9341_ini(uint16_t w_size, uint16_t h_size);

uint16_t ILI9341_RandColor(void);
void ILI9341_SetRotation(uint8_t r);
//-------------------------------------------------------------------
#endif /* __SPI_ILI9341_H */
