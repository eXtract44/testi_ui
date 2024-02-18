#include "spi_ili9341.h"
#include "main.h"

//from GitHub


extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi1;
//extern RNG_HandleTypeDef hrng;

uint16_t ILI9341_WIDTH;
uint16_t ILI9341_HEIGHT;
//-------------------------------------------------------------------

//-------------------------------------------------------------------brightness control
float map_ili(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void ILI9341_SetBrightness(uint8_t brightness)
{
	if (brightness < 1)
		brightness = 1;
	if (brightness > 100)
		brightness = 100;
	float i  = map_ili(brightness, 1, 100, 65000, 35000);

	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, i);
//for (uint32_t i = 0; i < 101; i += 1) {
//	ILI9341_SetBrightness(i);
//	HAL_Delay(75);
//}
}
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
void ILI9341_Write_Command(uint8_t Command) {
	CS_ACTIVE();
	DC_COMMAND();
	HAL_SPI_Transmit(&hspi1, &Command, 1, 1);
	CS_IDLE();
}
/* Send Data (char) to LCD */
void ILI9341_Write_Data(uint8_t Data) {
	DC_DATA();
	CS_ACTIVE();
	HAL_SPI_Transmit(&hspi1, &Data, 1, 1);
	CS_IDLE();
}
void ILI9341_Set_Address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	ILI9341_Write_Command(0x2A);
	ILI9341_Write_Data(x1 >> 8);
	ILI9341_Write_Data(x1);
	ILI9341_Write_Data(x2 >> 8);
	ILI9341_Write_Data(x2);

	ILI9341_Write_Command(0x2B);
	ILI9341_Write_Data(y1 >> 8);
	ILI9341_Write_Data(y1);
	ILI9341_Write_Data(y2 >> 8);
	ILI9341_Write_Data(y2);

	ILI9341_Write_Command(0x2C);
} //-------------------------------------------------------------------
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
	//ILI9341_fontsIni();
	ILI9341_SetRotation(3);
	//ILI9341_SetBrightness(100);
}
