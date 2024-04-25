#include "spi_ili9341.h"
#include "pic.c"
#include "main.h"
#include "math.h"
#include "adc.h"

extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi1;
//extern RNG_HandleTypeDef hrng;
uint16_t ILI9341_WIDTH;
uint16_t ILI9341_HEIGHT;
//-------------------------------------------------------------------
//-------------------------------------------------------------------DMA
extern uint8_t dma_spi_fl;
extern uint32_t dma_spi_cnt;
uint8_t frm_buf[65536] = { 0 };
//-------------------------------------------------------------------

typedef struct {
	uint16_t TextColor;
	uint16_t BackColor;
	sFONT *pFont;
} LCD_DrawPropTypeDef;
LCD_DrawPropTypeDef lcdprop;
//-------------------------------------------------------------------brightness control
float map1(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//char str[20];
void ILI9341_SetBrightness(uint8_t brightness)
//ini PWM Timer in (main.c /* USER CODE BEGIN 2 */) HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
{
	float i = 0;
	i = map1(brightness, 1, 100, 65000, 35000);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, i);

//for (uint32_t i = 0; i < 101; i += 1) {
//	ILI9341_SetBrightness(i);
//	HAL_Delay(75);
//}
}
//-------------------------------------------------------------------
//void ILI9341_WriteCmd8Data(uint8_t Cmd, uint16_t Data)
//{
//  ILI9341_SendCommand(Cmd);
//  ILI9341_SendData(Data);
//}
//-------------------------------------------------------------------
void ILI9341_SendCommand(uint8_t cmd) {
	DC_COMMAND();
	HAL_SPI_Transmit(&hspi1, &cmd, 1, 5000);
}
//-------------------------------------------------------------------
void ILI9341_SendData(uint8_t dt) {
	DC_DATA();
	HAL_SPI_Transmit(&hspi1, &dt, 1, 5000);
	//HAL_SPI_Transmit(&hspi1, &dt, 1, 5000);
	//HAL_SPI_Transmit_DMA (&hspi1, &dt, 1);
}
//-------------------------------------------------------------------
static void ILI9341_WriteData(uint8_t *buff, size_t buff_size) {
	DC_DATA();
	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
		HAL_SPI_Transmit(&hspi1, buff, chunk_size, HAL_MAX_DELAY);
		buff += chunk_size;
		buff_size -= chunk_size;
	}
}
//-------------------------------------------------------------------
void ILI9341_reset(void) {
	RESET_ACTIVE();
	HAL_Delay(5);
	RESET_IDLE();
}
//-------------------------------------------------------------------
static void ILI9341_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1,
		uint16_t y1) {
	// column address set
	ILI9341_SendCommand(0x2A); // CASET
	{
		uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1
				& 0xFF };
		ILI9341_WriteData(data, sizeof(data));
	}

	// row address set
	ILI9341_SendCommand(0x2B); // RASET
	{
		uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1
				& 0xFF };
		ILI9341_WriteData(data, sizeof(data));
	}

	// write to RAM
	ILI9341_SendCommand(0x2C); // RAMWR
}
//-------------------------------------------------------------------
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size) {
	//SENDS COLOUR
	uint32_t Buffer_Size = 0;
	if ((Size * 2) < BURST_MAX_SIZE) {
		Buffer_Size = Size;
	} else {
		Buffer_Size = BURST_MAX_SIZE;
	}

	DC_DATA();

	unsigned char chifted = Colour >> 8;
	unsigned char burst_buffer[Buffer_Size];
	for (uint32_t j = 0; j < Buffer_Size; j += 2) {
		burst_buffer[j] = chifted;
		burst_buffer[j + 1] = Colour;
	}

	uint32_t Sending_Size = Size * 2;
	uint32_t Sending_in_Block = Sending_Size / Buffer_Size;
	uint32_t Remainder_from_block = Sending_Size % Buffer_Size;

	if (Sending_in_Block != 0) {
		for (uint32_t j = 0; j < (Sending_in_Block); j++) {
			HAL_SPI_Transmit(&hspi1, (unsigned char*) burst_buffer, Buffer_Size,
			HAL_MAX_DELAY);
		}
	}

	//REMAINDER!
	HAL_SPI_Transmit(&hspi1, (unsigned char*) burst_buffer,
			Remainder_from_block, HAL_MAX_DELAY);
}

//-------------------------------------------------------------------

void ILI9341_fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
		uint16_t color) {
	//---DMA
//	if ((x1 >= ILI9341_WIDTH) || (y1 >= ILI9341_HEIGHT) || (x2 >= ILI9341_WIDTH)
//			|| (y2 >= ILI9341_HEIGHT))
//		return;
//	uint32_t i, n, cnt, buf_size;
//	if (x1 > x2)
//		swap(x1, x2);
//	if (y1 > y2)
//		swap(y1, y2);
//	ILI9341_SetAddrWindow(x1, y1, x2, y2);
//	DC_DATA();
//	n = (x2 - x1 + 1) * (y2 - y1 + 1) * 2; // pixel calculate
//	if (n <= 65535) {
//		cnt = 1;
//		buf_size = n;
//	} else {
//		cnt = n / 2;
//		buf_size = 2;
//		for (i = 3; i < n / 3; i++) {
//			if (n % i == 0) {
//				cnt = i;
//				buf_size = n / i;
//				break;
//			}
//		}
//	}
//	for (i = 0; i < buf_size / 2; i++) {
//		frm_buf[i * 2] = color >> 8;
//		frm_buf[i * 2 + 1] = color & 0xFF;
//	}
//	dma_spi_cnt = cnt;
//	HAL_SPI_Transmit_DMA(&hspi1, frm_buf, buf_size);
//
//	while (!dma_spi_fl) {
//	}
//	// if(!dma_spi_fl >0)
//	dma_spi_fl = 0;
	//---NO DMA
//
	if ((x1 >= ILI9341_WIDTH) || (y1 >= ILI9341_HEIGHT) || (x2 >= ILI9341_WIDTH)
			|| (y2 >= ILI9341_HEIGHT))
		return;
	if (x1 > x2)
		swap(x1, x2);
	if (y1 > y2)
		swap(y1, y2);
	ILI9341_SetAddrWindow(x1, y1, x2, y2);
	ILI9341_Draw_Colour_Burst(color, (x2 - x1 + 1) * (y2 - y1 + 1));
}
//-------------------------------------------------------------------
void ILI9341_fill(uint16_t color) {
	//******SPI DMA
//	uint32_t i, n;
//	ILI9341_SetAddrWindow(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
//	for (i = 0; i < 25600; i++) {
//		frm_buf[i * 2] = color >> 8; //
//		frm_buf[i * 2 + 1] = color & 0xFF;
//	}
//	n = 51200;
//	DC_DATA();
//	dma_spi_cnt = 3;
//	HAL_SPI_Transmit_DMA(&hspi1, frm_buf, n);
//	while (!dma_spi_fl) {
//	}
//	dma_spi_fl = 0;
	//******SPI Normal
	ILI9341_fillRect(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1, color);
}
//-------------------------------------------------------------------
void ILI9341_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
	if ((x < 0) || (y < 0) || (x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
		return;
	ILI9341_SetAddrWindow(x, y, x, y);
	ILI9341_SendCommand(0x2C);
	ILI9341_SendData(color >> 8);
	ILI9341_SendData(color & 0xFF);
}
//-------------------------------------------------------------------
//void ILI9341_Draw_Number(uint8_t number,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
//{
//	ILI9341_SetAddrWindow(x1, y1, x2, y2);
//	DC_DATA();
//
//	uint32_t counter = 0;
//
//	unsigned char Temp_small_buffer[BURST_MAX_SIZE];
//
//	for(uint32_t i = 0; i < 12816/BURST_MAX_SIZE; i++)
//	{
//		for(uint32_t k = 0; k<BURST_MAX_SIZE; k=k+2)
//		{
//			if((image_data_Font[number][counter/8]>>(7-(counter%8)))&0x01 == 1)
//			{
//				Temp_small_buffer[k] = 0x00;
//				Temp_small_buffer[k+1] = 0x00;
//			}
//			else
//			{
//				Temp_small_buffer[k] = (color>>8)&0xff;
//				Temp_small_buffer[k+1] = color&0xff;
//			}
//			counter++;
//		}
//		HAL_SPI_Transmit(&hspi1, (unsigned char*)Temp_small_buffer, BURST_MAX_SIZE, 10);
//	}
//
//	unsigned char Temp_small_buffer_2[316];
//
//	for(uint32_t k = 0; k<316; k=k+2)
//	{
//		if((image_data_Font[number][counter/8]>>(7-(counter%8)))&0x01 == 1)
//		{
//			Temp_small_buffer_2[k] = 0x00;
//			Temp_small_buffer_2[k+1] = 0x00;
//		}
//		else
//		{
//			Temp_small_buffer_2[k] = (color>>8)&0xff;
//			Temp_small_buffer_2[k+1] = color&0xff;
//		}
//		counter++;
//	}
//	HAL_SPI_Transmit(&hspi1, (unsigned char*)Temp_small_buffer_2, 316, 10);
//
//}

uint16_t ILI9341_RandColor(void) {
	return rand() & 0x0000FFFF;
}

//-------------------------------------------------------------------
void ILI9341_SetRotation(uint8_t r) {
	ILI9341_SendCommand(0x36);
	switch (r) {
	case 0:
		ILI9341_SendData(0x48);
		ILI9341_WIDTH = 240;
		ILI9341_HEIGHT = 320;
		break;
	case 1:
		ILI9341_SendData(0x28);
		ILI9341_WIDTH = 320;
		ILI9341_HEIGHT = 240;
		break;
	case 2:
		ILI9341_SendData(0x88);
		ILI9341_WIDTH = 240;
		ILI9341_HEIGHT = 320;
		break;
	case 3:
		ILI9341_SendData(0xE8);
		ILI9341_WIDTH = 320;
		ILI9341_HEIGHT = 240;
		break;
	}
}
//-----------------------------------------------------------------------------
void ILI9341_Delay(uint32_t dly) {
	uint32_t i;
	for (i = 0; i < dly; i++)
		;
}
//-------------------------------------------------------------------
__STATIC_INLINE void DelayMicro(uint32_t __IO micros) {
	micros *= (SystemCoreClock / 1600000) / 5;
	while (micros--)
		;
}
//-------------------------------------------------------------------
void DelaySeconds(uint32_t __IO seconds) {

	uint32_t i;
	seconds = seconds * 10000000;
	for (i = 0; i < seconds; i++)
		;
}
//-------------------------------------------------------------------
void DelayMili(uint32_t __IO mili) {

	uint32_t i;
	mili *= 10000;
	for (i = 0; i < mili; i++)
		;
}
//-------------------------------------------------------------------
void ILI9341_fontsIni(void) {
	Font8.Height = 8;
	Font8.Width = 5;
	Font12.Height = 12;
	Font12.Width = 7;
	Font16.Height = 16;
	Font16.Width = 11;
	Font20.Height = 20;
	Font20.Width = 14;
	Font24.Height = 24;
	Font24.Width = 17;
	lcdprop.BackColor = COLOR_BLACK;
	lcdprop.TextColor = COLOR_GREEN;
	lcdprop.pFont = &Font16;
}
//-------------------------------------------------------------------
void ILI9341_setFont(sFONT *pFonts) {
	lcdprop.pFont = pFonts;
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void ILI9341_drawRect(unsigned int x0, unsigned int y0, unsigned int x1,
		unsigned int y1, uint16_t color) {
	ILI9341_drawLine(x0, y0, x1, y0, color);
	ILI9341_drawLine(x1, y0, x1, y1, color);
	ILI9341_drawLine(x0, y0, x0, y1, color);
	ILI9341_drawLine(x0, y1, x1, y1, color);
}
//-------------------------------------------------------------------
//void ILI9341_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
//		int16_t r, uint16_t color) {
//	int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
//	if (r > max_radius)
//		r = max_radius;
//	// smarter version
//	ILI9341_drawFastHLine(x + r, y, w - 2 * r, color);         // Top
//	ILI9341_drawFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
//	ILI9341_drawFastVLine(x, y + r, h - 2 * r, color);         // Left
//	ILI9341_drawFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
//	// draw four corners
//	drawCircleHelper(x + r, y + r, r, 1, color);
//	drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
//	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
//	drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
//}
//-------------------------------------------------------------------
void ILI9341_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
		int16_t r, uint16_t color) {
	int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
	if (r > max_radius)
		r = max_radius;
	// smarter version
	ILI9341_fillRect(x + r, y, w - 2 * r, h, color);
	// draw four corners
	fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);

}
//-------------------------------------------------------------------
void ILI9341_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ILI9341_drawPixel(x0, y0 + r, color);
	ILI9341_drawPixel(x0, y0 - r, color);
	ILI9341_drawPixel(x0 + r, y0, color);
	ILI9341_drawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ILI9341_drawPixel(x0 + x, y0 + y, color);
		ILI9341_drawPixel(x0 - x, y0 + y, color);
		ILI9341_drawPixel(x0 + x, y0 - y, color);
		ILI9341_drawPixel(x0 - x, y0 - y, color);
		ILI9341_drawPixel(x0 + y, y0 + x, color);
		ILI9341_drawPixel(x0 - y, y0 + x, color);
		ILI9341_drawPixel(x0 + y, y0 - x, color);
		ILI9341_drawPixel(x0 - y, y0 - x, color);
	}
}
//-------------------------------------------------------------------
//static void drawCircleHelper(int16_t x0, int16_t y0, int16_t r,
//		uint8_t cornername, uint16_t color) {
//	int16_t f = 1 - r;
//	int16_t ddF_x = 1;
//	int16_t ddF_y = -2 * r;
//	int16_t x = 0;
//	int16_t y = r;
//
//	while (x < y) {
//		if (f >= 0) {
//			y--;
//			ddF_y += 2;
//			f += ddF_y;
//		}
//		x++;
//		ddF_x += 2;
//		f += ddF_x;
//		if (cornername & 0x4) {
//			ILI9341_drawPixel(x0 + x, y0 + y, color);
//			ILI9341_drawPixel(x0 + y, y0 + x, color);
//		}
//		if (cornername & 0x2) {
//			ILI9341_drawPixel(x0 + x, y0 - y, color);
//			ILI9341_drawPixel(x0 + y, y0 - x, color);
//		}
//		if (cornername & 0x8) {
//			ILI9341_drawPixel(x0 - y, y0 + x, color);
//			ILI9341_drawPixel(x0 - x, y0 + y, color);
//		}
//		if (cornername & 0x1) {
//			ILI9341_drawPixel(x0 - y, y0 - x, color);
//			ILI9341_drawPixel(x0 - x, y0 - y, color);
//		}
//	}
//}
//-------------------------------------------------------------------
static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
		uint8_t cornername, int16_t delta, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			ILI9341_drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			ILI9341_drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if (cornername & 0x2) {
			ILI9341_drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			ILI9341_drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}
//-------------------------------------------------------------------
void ILI9341_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	ILI9341_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}
//-------------------------------------------------------------------
void ILI9341_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		uint16_t color) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			ILI9341_drawPixel(y0, x0, color);
		} else {
			ILI9341_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}
//-------------------------------------------------------------------
void ILI9341_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	ILI9341_drawLine(x, y, x + w - 1, y, color);
}
//-------------------------------------------------------------------
void ILI9341_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	ILI9341_drawLine(x, y, x, y + h - 1, color);
}
//-------------------------------------------------------------------
void ILI9341_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		int16_t x2, int16_t y2, uint16_t color) {
	ILI9341_drawLine(x0, y0, x1, y1, color);
	ILI9341_drawLine(x1, y1, x2, y2, color);
	ILI9341_drawLine(x2, y2, x0, y0, color);
}
//-------------------------------------------------------------------
void ILI9341_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
		int16_t x2, int16_t y2, uint16_t color) {
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1);
		swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		ILI9341_drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
			dx12 = x2 - x1, dy12 = y2 - y1, sa = 0, sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
		last = y1;   // Include y1 scanline
	else
		last = y1 - 1; // Skip it

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b)
			swap(a, b);
		ILI9341_drawFastHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b)
			swap(a, b);
		ILI9341_drawFastHLine(a, y, b - a + 1, color);
	}
}
//-------------------------------------------------------------------
void ILI9341_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
		uint16_t bg, uint8_t size) {
//	if ((x >= ILI9341_HEIGHT) || // Clip right
//			(y >= ILI9341_WIDTH) || // Clip bottom
//			((x + 6 * size - 1) < 0) || // Clip left
//			((y + 8 * size - 1) < 0))   // Clip top
//		return;

//	if (!_cp437 && (c >= 176))
//		c++; // Handle 'classic' charset behavior

	for (int8_t i = 0; i < 6; i++) {
		uint8_t line;
		if (i == 5)
			line = 0x0;
		else
			line = pgm_read_byte(font1 + (c * 5) + i);
		for (int8_t j = 0; j < 8; j++) {
			if (line & 0x1) {
				if (size == 1) // default size
					ILI9341_drawPixel(x + i, y + j, color);
				else {  // big size
					ILI9341_fillRect(x + (i * size), y + (j * size),
							size + x + (i * size), size + 1 + y + (j * size),
							color);
				}
			} else if (bg != color) {
				if (size == 1) // default size
					ILI9341_drawPixel(x + i, y + j, bg);
				else {  // big size
					ILI9341_fillRect(x + i * size, y + j * size,
							size + x + i * size, size + 1 + y + j * size, bg);
				}
			}
			line >>= 1;
		}
	}
}
//-------------------------------------------------------------------
void ILI9341_drawCharF(uint16_t x, uint16_t y, uint8_t c, uint16_t color,
		uint16_t bg) {

	uint32_t i = 0, j = 0;
	uint16_t height, width;
	uint8_t offset;
	uint8_t *c_t;
	uint8_t *pchar;
	uint32_t line = 0;
	height = lcdprop.pFont->Height;
	width = lcdprop.pFont->Width;
	offset = 8 * ((width + 7) / 8) - width;
	c_t = (uint8_t*) &(lcdprop.pFont->table[(c - ' ') * lcdprop.pFont->Height
			* ((lcdprop.pFont->Width + 7) / 8)]);
	for (i = 0; i < height; i++) {
		pchar = ((uint8_t*) c_t + (width + 7) / 8 * i);
		switch (((width + 7) / 8)) {
		case 1:
			line = pchar[0];
			break;
		case 2:
			line = (pchar[0] << 8) | pchar[1];
			break;
		case 3:
		default:
			line = (pchar[0] << 16) | (pchar[1] << 8) | pchar[2];
			break;
		}
		for (j = 0; j < width; j++) {
			if (line & (1 << (width - j + offset - 1))) {
				ILI9341_drawPixel((x + j), y, color);
			} else {
				ILI9341_drawPixel((x + j), y, bg);
			}
		}
		y++;
	}
}
//-------------------------------------------------------------------
void ILI9341_string(uint16_t x, uint16_t y, char *str, uint16_t color,
		uint16_t bg) {
	while (*str) {
		ILI9341_drawCharF(x, y, str[0], color, bg);
		x += lcdprop.pFont->Width;
		(void) *str++;
	}
}
//------------------------------------------------------------------
void ILI9341_printText(char text[], int16_t x, int16_t y, uint16_t color,
		uint16_t bg, uint8_t size) {
	int16_t offset;
	offset = size * 6;
	for (uint16_t i = 0; i < 40 && text[i] != NULL; i++) {
		ILI9341_drawChar(x + (offset * i), y, text[i], color, bg, size);
	}
}
//-------------------------------------------------------------------
//12. Image print (RGB 565, 2 bytes per pixel)
void ILI9341_drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w,
		int16_t h, uint16_t color) {

	ILI9341_SetAddrWindow(x, y, x + w, y + h);
	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;

	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = bitmap[j * byteWidth + i / 8];
			if (byte & 0x80)
				ILI9341_drawPixel(x + i, y, color);
		}
	}

}
void ILI9341_drawBitmapFast(int16_t x, int16_t y, const uint8_t *bitmap,
		int16_t w, int16_t h, uint16_t color, uint32_t size) {
	uint8_t data_0[] = { color >> 8, color & 0xFF }; //color black
	uint8_t data_1[] = { COLOR_MENU_BG >> 8, COLOR_MENU_BG & 0xFF }; //color white
	int8_t z = 0;
	//for (uint32_t z = 0; z < 10; z++) {///****
	if (w == 80)
		z = -1;
	if (w == 71)
		z = 0;
	if (w == 66)
		z = 5;
	if (w < 65)
		z = 6;
	if (w == 20 && h == 20)
		z = 3;

	if (w == 35 && h == 30)
		z = 4;

	if (w == 50 && h == 50)
		z = 5;
	if (w == 55 && h == 55)
		z = 0;

	if (w == 56 && h == 112)
		z = -1;
	if (w == 100 && h == 77)
		z = 3;
	//if(w == 33)z = 6;
	//if(w < 33)z = 6;
	ILI9341_SetAddrWindow(x, y, x + w + z, y + h); //
	//COLOR_MENU_BG
	DC_DATA();
	for (uint32_t i = 0; i < size; i++) { //sizeof(bitmap)
		for (uint32_t j = 0; j < 8; j++) {
			if (((bitmap[i] >> (7 - j)) & 0x01) == 1)
				HAL_SPI_Transmit(&hspi1, data_0, 2, HAL_MAX_DELAY);
			else
				//byte = bitmap[i * byteWidth + j / 8];
				HAL_SPI_Transmit(&hspi1, data_1, 2, HAL_MAX_DELAY);
		}
	}
	//HAL_Delay(500);
	//}
}
void ILI9341_drawBitmapFastBar(int16_t x, int16_t y, const uint8_t *bitmap,
		int16_t w, int16_t h, uint16_t color, uint32_t size) {
	uint8_t data_0[] = { color >> 8, color & 0xFF }; //color black
	uint8_t data_1[] = { COLOR_MENU_BAR_DOWN >> 8, COLOR_MENU_BAR_DOWN & 0xFF }; //color white
	int8_t z = 0;
	//for (uint32_t z = 0; z < 10; z++) {///****
	if (w == 80)
		z = -1;
	if (w == 71)
		z = 0;
	if (w == 66)
		z = 5;
	if (w < 65)
		z = 6;
	if (w == 20 && h == 20)
		z = 3;

	if (w == 35 && h == 30)
		z = 4;

	if (w == 50 && h == 50)
		z = 5;
	if (w == 55 && h == 55)
		z = 0;

	if (w == 56 && h == 112)
		z = -1;
	if (w == 100 && h == 77)
		z = 3;
	//if(w == 33)z = 6;
	//if(w < 33)z = 6;
	ILI9341_SetAddrWindow(x, y, x + w + z, y + h); //
	//COLOR_MENU_BG
	DC_DATA();
	for (uint32_t i = 0; i < size; i++) { //sizeof(bitmap)
		for (uint32_t j = 0; j < 8; j++) {
			if (((bitmap[i] >> (7 - j)) & 0x01) == 1)
				HAL_SPI_Transmit(&hspi1, data_0, 2, HAL_MAX_DELAY);
			else
				//byte = bitmap[i * byteWidth + j / 8];
				HAL_SPI_Transmit(&hspi1, data_1, 2, HAL_MAX_DELAY);
		}
	}
	//HAL_Delay(500);
	//}
}

/*
void ILI9341_draw_Number(uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2,
		uint16_t y2, uint16_t color) {

	 TFT9341_SetAddrWindow(x1, y1, x2, y2);
	 DC_DATA();

	 uint32_t counter = 0;

	 unsigned char Temp_small_buffer[BURST_MAX_SIZE];

	 for(uint32_t i = 0; i < 12816/BURST_MAX_SIZE; i++)
	 {
	 for(uint32_t k = 0; k<BURST_MAX_SIZE; k=k+2)
	 {
	 if((image_data_Font[number][counter/8]>>(7-(counter%8)))&0x01 == 1)
	 {
	 Temp_small_buffer[k] = 0x00;
	 Temp_small_buffer[k+1] = 0x00;
	 }
	 else
	 {
	 Temp_small_buffer[k] = (color>>8)&0xff;
	 Temp_small_buffer[k+1] = color&0xff;
	 }
	 counter++;
	 }
	 HAL_SPI_Transmit(&hspi1, (unsigned char*)Temp_small_buffer, BURST_MAX_SIZE, 10);
	 }

	 unsigned char Temp_small_buffer_2[316];

	 for(uint32_t k = 0; k<316; k=k+2)
	 {
	 if((image_data_Font[number][counter/8]>>(7-(counter%8)))&0x01 == 1)
	 {
	 Temp_small_buffer_2[k] = 0x00;
	 Temp_small_buffer_2[k+1] = 0x00;
	 }
	 else
	 {
	 Temp_small_buffer_2[k] = (color>>8)&0xff;
	 Temp_small_buffer_2[k+1] = color&0xff;
	 }
	 counter++;
	 }
	 HAL_SPI_Transmit(&hspi1, (unsigned char*)Temp_small_buffer_2, 316, 10);

}
*/
void ILI9341_printImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		uint8_t *data, uint32_t size) {
	uint32_t n = size;
	ILI9341_SetAddrWindow(x, y, w + x - 1, h + y - 1);
	for (uint32_t i = 0; i < n; i++) {
		ILI9341_SendData(data[i]);
	}
}
//-------------------------------------------------------------------
void ILI9341_ini(uint16_t w_size, uint16_t h_size) {
	uint8_t data[15];
	CS_ACTIVE();
	ILI9341_reset();
	//Software Reset
	ILI9341_SendCommand(0x01);
	HAL_Delay(1000);
	//Power Control A
	data[0] = 0x39;
	data[1] = 0x2C;
	data[2] = 0x00;
	data[3] = 0x34;
	data[4] = 0x02;
	ILI9341_SendCommand(0xCB);
	ILI9341_WriteData(data, 5);
	//Power Control B
	data[0] = 0x00;
	data[1] = 0xC1;
	data[2] = 0x30;
	ILI9341_SendCommand(0xCF);
	ILI9341_WriteData(data, 2);
	//Driver timing control A
	data[0] = 0x85;
	data[1] = 0x00;
	data[2] = 0x78;
	ILI9341_SendCommand(0xE8);
	ILI9341_WriteData(data, 2);
	//Driver timing control B
	data[0] = 0x00;
	data[1] = 0x00;
	ILI9341_SendCommand(0xEA);
	ILI9341_WriteData(data, 2);
	//Power on Sequence control
	data[0] = 0x64;
	data[1] = 0x03;
	data[2] = 0x12;
	data[3] = 0x81;
	ILI9341_SendCommand(0xED);
	ILI9341_WriteData(data, 4);
	//Pump ratio control
	data[0] = 0x20;
	ILI9341_SendCommand(0xF7);
	ILI9341_WriteData(data, 1);
	//Power Control,VRH[5:0]
	data[0] = 0x10;
	ILI9341_SendCommand(0xC0);
	ILI9341_WriteData(data, 1);
	//Power Control,SAP[2:0];BT[3:0]
	data[0] = 0x10;
	ILI9341_SendCommand(0xC1);
	ILI9341_WriteData(data, 1);
	//VCOM Control 1
	data[0] = 0x3E;
	data[1] = 0x28;
	ILI9341_SendCommand(0xC5);
	ILI9341_WriteData(data, 2);
	//VCOM Control 2
	data[0] = 0x86;
	ILI9341_SendCommand(0xC7);
	ILI9341_WriteData(data, 1);
	//Memory Acsess Control
	data[0] = 0x48;
	ILI9341_SendCommand(0x36);
	ILI9341_WriteData(data, 1);
	//Pixel Format Set
	data[0] = 0x55; //16bit
	ILI9341_SendCommand(0x3A);
	ILI9341_WriteData(data, 1);
	//Frame Rratio Control, Standard RGB Color
	data[0] = 0x00;
	data[1] = 0x18;
	ILI9341_SendCommand(0xB1);
	ILI9341_WriteData(data, 2);
	//Display Function Control
	data[0] = 0x08;
	data[1] = 0x82;
	data[2] = 0x27; //320 строк
	ILI9341_SendCommand(0xB6);
	ILI9341_WriteData(data, 2);
	//Enable 3G (пока не знаю что это за режим)
	data[0] = 0x00; //не включаем
	ILI9341_SendCommand(0xF2);
	ILI9341_WriteData(data, 1);
	//Gamma set
	data[0] = 0x01; //Gamma Curve (G2.2) (Кривая цветовой гаммы)
	ILI9341_SendCommand(0x26);
	ILI9341_WriteData(data, 1);
	//Positive Gamma  Correction
	data[0] = 0x0F;
	data[1] = 0x31;
	data[2] = 0x2B;
	data[3] = 0x0C;
	data[4] = 0x0E;
	data[5] = 0x08;
	data[6] = 0x4E;
	data[7] = 0xF1;
	data[8] = 0x37;
	data[9] = 0x07;
	data[10] = 0x10;
	data[11] = 0x03;
	data[12] = 0x0E;
	data[13] = 0x09;
	data[14] = 0x00;
	ILI9341_SendCommand(0xE0);
	ILI9341_WriteData(data, 15);
	//Negative Gamma  Correction
	data[0] = 0x00;
	data[1] = 0x0E;
	data[2] = 0x14;
	data[3] = 0x03;
	data[4] = 0x11;
	data[5] = 0x07;
	data[6] = 0x31;
	data[7] = 0xC1;
	data[8] = 0x48;
	data[9] = 0x08;
	data[10] = 0x0F;
	data[11] = 0x0C;
	data[12] = 0x31;
	data[13] = 0x36;
	data[14] = 0x0F;
	ILI9341_SendCommand(0xE1);
	ILI9341_WriteData(data, 15);
	ILI9341_SendCommand(0x11); //Выйдем из спящего режима
	HAL_Delay(120);
	//Display ON
	data[0] = ILI9341_ROTATION;

	ILI9341_SendCommand(0x29);
	ILI9341_WriteData(data, 1);
	ILI9341_WIDTH = w_size;
	ILI9341_HEIGHT = h_size;
	ILI9341_fontsIni();
	ILI9341_SetRotation(3);
	ILI9341_SetBrightness(100);

}


