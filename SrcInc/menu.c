
#include "stdint.h"
#include "stdbool.h"

#include "menu.h"
#include "spi_ili9341.h"
#include "pic.h"

extern uint8_t current_pt100_page;
extern uint16_t cy_old;
extern bool running;

extern uint16_t ILI9341_WIDTH;
extern uint16_t ILI9341_HEIGHT;
extern uint8_t standart_helligkeit;

uint16_t frame_val_ystart = 64;
uint16_t frame_val_ysize = 30;
uint16_t frame_val_xstart = 206;

uint16_t frame_val_ytext = 64 + 4;
uint16_t frame_val_xtext = 206 - 43;
uint16_t frame_val_xtext_m = 206 - 43 - 65;

//--for print adc values
uint16_t adc_val_xstart = 206 + 5;
uint16_t mess_val_xstart = 206 - 100; //
uint16_t adc_val_ystart = 64 + 4;

//---------------------Menu buttons position x start , x finish  , button length = 53
uint16_t x_pos_button_l = 20;
uint16_t y_pos_button_l = 210; //

uint16_t x_pos_button_r = 247; // 247
uint16_t y_pos_button_r = 210;

uint16_t x_pos_button_c = 150;
uint16_t y_pos_button_c = 210;

uint16_t x_pos_batt = 260;
uint16_t y_pos_batt = 40;

uint8_t page_main_menu = 1;
uint8_t page_main_menu_old;
uint8_t page_sensor_menu = 1;
uint8_t page_antrieb_menu = 1;
uint8_t page_info_menu = 1;

//-------------------------------------------------------------------GUI Menu
//-------------------------------------------------------------------Val

#define text_menu_size &Font16
#define menu_bar_up 0, 0, 319, 39, COLOR_MENU_BAR_DOWN
#define menu_bar_down 0, 200, 319, 239, COLOR_MENU_BAR_DOWN

//-------------------------------------------------------------------

/*******************************/

/*******************************/
void drw_circle_raw(unsigned int x, unsigned int y, unsigned int r, unsigned int frame_size, uint16_t color, uint16_t color_bg)
{
	ILI9341_fillCircle(x, y, r, color_bg);
	for (uint8_t i = 0; i < 3; i++)
	{
		ILI9341_drawCircle(x, y, i, color);
	}
}
void drw_frame_raw(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int frame_size, uint16_t color, uint16_t color_bg)
 {
	ILI9341_fillRect(x, y, x + w, y + h, color_bg);

	for (uint8_t i = frame_size; i == 0; i--)
	{
		ILI9341_drawRect(frame_size / 2 + x - i, y - i, frame_size / 2 + x + w + i, y + h + i, color);
	}
}
bool drw_btn_raw(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int frame_size, uint16_t color, uint16_t color_bg)
{
	bool passed = 1;
	if (w > MAX_WIDTH_SIZE_BTN)
	{
		passed = 0;
		w = MAX_WIDTH_SIZE_BTN;
	}

	if (h > MAX_HEIGHT_SIZE_BTN)
	{
		passed = 0;
		h = MAX_HEIGHT_SIZE_BTN;
	}

	if (frame_size > MAX_FRAME_SIZE_BTN)
	{
		passed = 0;
		frame_size = MAX_FRAME_SIZE_BTN;
	}
	drw_frame_raw(x, y, w, h, frame_size, color, color_bg);
	return passed;
}
void drw_btn_text(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t frame_size, uint16_t color, uint16_t color_bg, char *text, uint16_t text_size, uint16_t color_text, uint16_t color_text_bg)
{
	if (drw_btn_raw(x, y, w, h, frame_size, color, color_bg) == true)
	{
		switch (text_size)
		{
		case 1:
			ILI9341_printText(text, x + w / 2 - 4, y + h / 2 - 5, color_text, color_text_bg, text_size);
			break;
		case 2:
			ILI9341_printText(text, x + w / 2 - 8, y + h / 2 - 9, color_text, color_text_bg, text_size);
			break;
		case 3:
			ILI9341_printText(text, x + w / 2 - 12, y + h / 2 - 12, color_text, color_text_bg, text_size);
			break;
		case default:
			break;
		}
	}
}
void drw_btn_text_large(int16_t x, int16_t y, uint16_t color, uint16_t color_bg, char *text, uint16_t text_size, uint16_t color_text, uint16_t color_text_bg)
{
	drw_btn_char(x, y, BTN_WIDTH_LARGE, BTN_HEIGHT_LARGE, 4, color, color_bg, text, 2, color_text, color_text_bg);
}
void drw_btn_text_medium(int16_t x, int16_t y, uint16_t color, uint16_t color_bg, char *text, uint16_t text_size, uint16_t color_text, uint16_t color_text_bg)
{
	drw_btn_char(x, y, BTN_WIDTH_MEDIUM, BTN_HEIGHT_MEDIUM, 4, color, color_bg, text, 2, color_text, color_text_bg);
}
void drw_btn_text_small(int16_t x, int16_t y, uint16_t color, uint16_t color_bg, char *text, uint16_t text_size, uint16_t color_text, uint16_t color_text_bg)
{
	drw_btn_char(x, y, BTN_WIDTH_SMALL, BTN_HEIGHT_SMALL, 4, color, color_bg, text, 2, color_text, color_text_bg);
}
void drw_btn_round(unsigned int x, unsigned int y, unsigned int r, unsigned int frame_size, uint16_t color, uint16_t color_bg)
{
	drw_circle_raw(x, y, r, frame_size, color, color_bg);
}
void drw_menu_bar_top(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int line_size, uint16_t color_bar, uint16_t color_line)
{
	ILI9341_fillRect(x, y, x + w, y + h, color_bar);
	ILI9341_fillRect(x, y + h, x + w, y, color_line);
}
void drw_menu_bar_bottom(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int line_size, uint16_t color_bar, uint16_t color_line)
{
	ILI9341_fillRect(x, y, x + w, y + h, color_bar);
	ILI9341_fillRect(x, y, x + w, y + h, color_line);
}
//-------------------------------------------------------------------
void drw_connection_diagram(uint16_t x, uint16_t y, , uint16_t color, uint16_t color_bg, uint16_t color_text, uint16_t color_text_bg)
{
	const uint16_t size = 38;
	const uint16_t high = 22;
	const uint8_t number_of_connections = 8;
	ILI9341_fillRect(x, y, x + DISPLAY_WIDTH - 5, y + high, color_bg);
	ILI9341_drawLine(x, y, x + DISPLAY_WIDTH - 5, y, color);
	ILI9341_drawLine(x, y + high, DISPLAY_WIDTH - 5, y + high, color);
	for (uint8_t i = 0; i < number_of_connections; i++)
	{
		ILI9341_drawLine(x + i * size, y, x + i * size, y + high, color);
	}
	ILI9341_printText("PT+ PT- GND IN2 IN1 YYY GND 24V " + 3, x, y + 7, color_text, color_text_bg, 1);
}
//-------------------------------------------------------------------
void drw_menu_main_bg(){
	
}
void drw_menu_home()
{
	power_24v_off();
	ILI9341_fill(COLOR_MENU_BG);
	drw_menu_bar_top(LEFT_CORNER_X, TOP_CORNER_Y, 320, 30, 3, COLOR_MENU_BAR, COLOR_WHITE);
	drw_menu_bar_bottom(LEFT_CORNER_X, 200, 320, 30, 3, COLOR_MENU_BAR, COLOR_WHITE);
	drw_btn_round(BTN_MID_BOTTOM_X, BTN_MID_BOTTOM_Y, 15, 3, COLOR_WHITE, COLOR_MENU_BAR);
	drw_menu_pic(MAIN_MENU);
	// read_adc_battery(x_pos_batt, y_pos_batt - 25);
	ILI9341_drawBitmapFastBar(15, 10, mcu_20x20, 20, 20, COLOR_YELLOW,
							  sizeof mcu_20x20);
}
//-------------------------------------------------------------------
void drw_menu_actor_010V()
{
	ILI9341_fill(COLOR_MENU_BG);
	drw_menu_bar_top(LEFT_CORNER_X, TOP_CORNER_Y, 320, 30, 3, COLOR_MENU_BAR, COLOR_WHITE);
	drw_menu_bar_bottom(LEFT_CORNER_X, 200, 320, 30, 3, COLOR_MENU_BAR, COLOR_WHITE);
	drw_frame_raw(frame_val_xstart, frame_val_ystart, 60, 30, 3, COLOR_WHITE, COLOR_MENU_BG);
	drw_frame_raw(frame_val_xstart, frame_val_ystart + frame_val_ysize, 60, 30, 3, COLOR_WHITE, COLOR_MENU_BG);
	drw_frame_raw(frame_val_xstart, frame_val_ystart + frame_val_ysize * 2, 60, 30, 3, COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_printText(" Y=", frame_val_xtext, frame_val_ytext, COLOR_WHITE,
					  COLOR_MENU_BG, 1);
	ILI9341_printText("IN1", frame_val_xtext, frame_val_ytext + frame_val_ysize,
					  COLOR_WHITE, COLOR_MENU_BG, 1);
	ILI9341_printText("Ub=", frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
					  COLOR_WHITE, COLOR_MENU_BG, 1);
	drw_connection_diagram(LEFT_CORNER_X, TOP_CORNER_Y + 5, , COLOR_BLACK, COLOR_MENU_BG, COLOR_BLUE, COLOR_MENU_BG);
	draw_start_button(COLOR_WHITE);
	read_adc_battery(x_pos_batt, y_pos_batt);
	power_24v_on();
}
//-------------------------------------------------------------------
void draw_menu_antrieb_s()
{														// Antriebtester S
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_SET); // 24VINTERN
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_SET); // 24VOUT
	ILI9341_SetBrightness(1);
	ILI9341_fill(COLOR_MENU_BG);
	ILI9341_fillRect(menu_bar_down);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 198 + i, 319, 198 + i, COLOR_WHITE);
	ILI9341_setFont(text_menu_size);
	//*** Draw frames for meas values
	draw_m_frame(frame_val_xstart, frame_val_ystart);
	draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize);
	draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize * 2);
	//*** Draw text for frames
	ILI9341_string(frame_val_xtext, frame_val_ytext, "AUF", COLOR_WHITE,
				   COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize, " ZU",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, antrieb_56x112, 56, 112, COLOR_WHITE,
						   sizeof antrieb_56x112);

	uint16_t xsta = 140;
	uint16_t ylen = 25;
	uint16_t xlen = 38;
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_drawLine(xsta + i, 20, xsta + i, 30 + ylen, COLOR_WHITE); // zu
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_drawLine(xsta, 30 + ylen - i, xsta + xlen + 6, 30 + ylen - i,
						 COLOR_WHITE); // zu
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_drawLine(xsta + xlen - i, 20, xsta + xlen - i, 20 + ylen,
						 COLOR_WHITE); // auf
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_drawLine(xsta + xlen * 2 + i, 20, xsta + xlen * 2 + i,
						 20 + ylen, COLOR_WHITE); //+
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_drawLine(xsta + xlen + 15, 20 + ylen + i, xsta + xlen * 2,
						 20 + ylen + i, COLOR_WHITE); //+
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_drawLine(xsta + xlen + 15, 20 + ylen + i, xsta + xlen + 2,
						 33 + ylen + i, COLOR_WHITE); //+
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_string(xsta - 25, 28, "S3", COLOR_WHITE, COLOR_MENU_BG);
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_string(xsta + xlen - 25, 28, "S2", COLOR_WHITE, COLOR_MENU_BG);
	for (uint8_t i = 0; i < 2; i++)
		ILI9341_string((xsta + xlen * 2) - 25, 28, "S1", COLOR_WHITE,
					   COLOR_MENU_BG);
	draw_legende(3, 2);
	draw_start_button(COLOR_WHITE);
	read_adc_battery(x_pos_batt, y_pos_batt);
}
//----------------------------------------------------
void draw_menu_sensor()
{ // sensor menu1
	// HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_SET); //24VINTERN
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_SET); // 24VOUT
	ILI9341_SetBrightness(1);
	ILI9341_fill(COLOR_MENU_BG);
	ILI9341_fillRect(menu_bar_down);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 198 + i, 319, 198 + i, COLOR_WHITE);
	ILI9341_setFont(text_menu_size);
	//*** Draw frames for meas values
	draw_m_frame(frame_val_xstart, frame_val_ystart);
	draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize);
	draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize * 2);
	//*** Draw text for frames
	ILI9341_string(frame_val_xtext, frame_val_ytext, "IN1", COLOR_WHITE,
				   COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);
	draw_legende(3, 3);
	draw_start_button(COLOR_WHITE);
	read_adc_battery(x_pos_batt, y_pos_batt);

	for (uint8_t i = 1; i < standart_helligkeit; i++)
	{
		DelayMili(10);
		ILI9341_SetBrightness(i);
	}
	DelayMili(delay_menus);
}
//-------------------------------------------------------------------
void draw_premenu_sensor()
{ // sensor menu1
	ILI9341_SetBrightness(1);
	ILI9341_fill(COLOR_MENU_BG);
	ILI9341_fillRect(0, 0, 319, 25, 0xe9);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 25 + i, 319, 25 + i, COLOR_WHITE);
	ILI9341_fillRect(menu_bar_down);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 198 + i, 319, 198 + i, COLOR_WHITE);
	//*** Draw frames for meas values
	draw_s_frame(frame_val_xstart - 90, frame_val_ystart);
	draw_m_frame(frame_val_xstart - 90, frame_val_ystart + frame_val_ysize * 2);
	//*** Draw text for frames
	ILI9341_printText("<MIN>", frame_val_xstart - 70, frame_val_ystart - 30,
					  COLOR_WHITE, COLOR_MENU_BG, 2);
	ILI9341_printText("<MAX>", frame_val_xstart - 70,
					  frame_val_ystart + frame_val_ysize * 2 - 30,
					  COLOR_WHITE, COLOR_MENU_BG, 2);

	draw_button_helper('l');
	draw_button_helper('r');

	draw_start_button(COLOR_WHITE);
	for (uint8_t i = 1; i < standart_helligkeit; i++)
	{
		DelayMili(10);
		ILI9341_SetBrightness(i);
	}
	DelayMili(delay_menus);
}
void draw_pt100(uint16_t x, uint16_t y, uint16_t color)
{
	uint8_t s = 2;
	for (uint8_t t = 0; t <= s; t++)
	{
		ILI9341_drawLine(x - t, y, x - t, y + 60, color); // left
	}
	for (uint8_t t = 0; t <= s; t++)
	{
		ILI9341_drawLine(x, y + 60 + t, x + 30, y + 60 + t, color); // down
	}
	for (uint8_t t = 0; t <= s; t++)
	{
		ILI9341_drawLine(x + 30 + t, y, x + 30 + t, y + 60, color); // right
	}
	for (uint8_t t = 0; t <= s; t++)
	{
		ILI9341_drawLine(x + 14 + t, y, x + 14 + t, y + 60, color); // mid
	}
	ILI9341_drawLine(x + 5, y + 15, x + 25, y + 45, color); // pin
	ILI9341_fillRect(x + 10, y + 10, x + 20, y + 50,		// Resistor
					 color);
	ILI9341_printText("PT+ PT- GND", x - 15, y - 10, COLOR_WHITE, COLOR_MENU_BG,
					  1);
}
//-------------------------------------------------------------------
void draw_poti(uint16_t x, uint16_t y, uint16_t color)
{
	uint8_t s = 2;
	for (uint8_t t = 0; t <= s; t++)
	{
		ILI9341_drawLine(x - t, y, x - t, y + 60, color); // left
	}
	for (uint8_t t = 0; t <= s; t++)
	{
		ILI9341_drawLine(x, y + 60 + t, x + 15, y + 60 + t, color); // down
	}
	for (uint8_t t = 0; t <= s; t++)
	{
		ILI9341_drawLine(x + 14 + t, y, x + 14 + t, y + 60, color); // mid
	}
	ILI9341_drawLine(x + 5, y + 15, x + 25, y + 45, color); // pin
	ILI9341_fillRect(x + 10, y + 10, x + 20, y + 50,		// Resistor
					 color);
	ILI9341_printText("IN1  Y", x - 10, y - 10, COLOR_WHITE, COLOR_MENU_BG, 1);
}
//-------------------------------------------------------------------
void draw_pt100menu()
{														// sensor menu
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_SET); // 24VINTERN
	// HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_SET); //24VOUT
	ILI9341_SetBrightness(1);
	ILI9341_fill(COLOR_MENU_BG);
	ILI9341_fillRect(menu_bar_down);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 198 + i, 319, 198 + i, COLOR_WHITE);

	draw_pt100(255, 75, COLOR_YELLOW);
	//-----grafik
	draw_graphic_m(30, 20, COLOR_WHITE);

	draw_start_button(COLOR_WHITE);
	draw_legende(3, 6);
	cy_old = 0;
	current_pt100_page = 1;
	read_adc_battery(x_pos_batt, y_pos_batt);

	for (uint8_t i = 1; i < standart_helligkeit; i++)
	{
		DelayMili(10);
		ILI9341_SetBrightness(i);
	}
	DelayMili(delay_menus);
}
//-------------------------------------------------------------------
void draw_start()
{
	ILI9341_fill(COLOR_MENU_BG);
	ILI9341_setFont(&Font24);
	ILI9341_printText("TESTI V0.2", 80, 120, COLOR_WHITE, COLOR_MENU_BG, 2);
}
//-------------------------------------------------------------------
void draw_infoMenu1()
{
	// ILI9341_SetBrightness(1);
	ILI9341_fill(COLOR_MENU_BG);
	// top bar
	ILI9341_fillRect(menu_bar_up);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 39 + i, 319, 39 + i, COLOR_WHITE);
	// down bar
	ILI9341_fillRect(menu_bar_down);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 198 + i, 319, 198 + i, COLOR_WHITE);

	ILI9341_setFont(&Font12);
	uint16_t color_text = COLOR_WHITE;
	uint16_t color_text_bg = COLOR_MENU_BG;
	uint16_t xStart = 5;
	uint16_t yStart = 60;
	uint16_t ySize = 20;

	ILI9341_string(xStart, yStart, "Betriebsspannung: 24VDC", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize, "Strom: 0.5A, Leistung: 10W",
				   color_text, color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 2, "Spannung Y/IN1/2: Max.24VDC",
				   color_text, color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 3, "Ladezeit:", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 4, "2A Ladegaeret ca. 2.5 Std",
				   color_text, color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 5, "USB Ladegaeret ca. 5 Std",
				   color_text, color_text_bg);

	// ILI9341_string(35, yStart+ySize*7, "MCU: STM32F407VET6 168MHz, 512Kb Flash", color_text, color_text_bg);
	ILI9341_drawBitmapFast(240, 90, logo_ga_55x55, 55, 55, COLOR_RED,
						   sizeof logo_ga_55x55);

	ILI9341_drawBitmapFastBar(15, 10, mcu_20x20, 20, 20, COLOR_YELLOW,
							  sizeof mcu_20x20);
	read_adc_battery(x_pos_batt, y_pos_batt - 25);
	//	for (uint8_t i = 1; i < 101; i++) {
	//		DelayMili(10);
	//		ILI9341_SetBrightness(i);
	//	}
	DelayMili(delay_menus);
}
//-------------------------------------------------------------------
void draw_menu_tools()
{
	ILI9341_SetBrightness(1);
	ILI9341_fill(COLOR_MENU_BG);
	// top bar
	ILI9341_fillRect(menu_bar_up);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 39 + i, 319, 39 + i, COLOR_WHITE);
	// down bar
	ILI9341_fillRect(menu_bar_down);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 198 + i, 319, 198 + i, COLOR_WHITE);

	ILI9341_setFont(&Font12);
	uint16_t color_text = COLOR_WHITE;
	uint16_t color_text_bg = COLOR_MENU_BG;
	uint16_t xStart = 5;
	uint16_t yStart = 60;
	uint16_t ySize = 22;

	ILI9341_string(xStart, yStart, "Helligkeit", color_text, color_text_bg);
	ILI9341_string(xStart, yStart + ySize, "Standby", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 2, "Standby Display OFF",
				   color_text, color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 3, "Standby ALL OFF", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 4, "Touch Ton", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 5, "Alarm Ton", color_text,
				   color_text_bg);

	read_adc_battery(x_pos_batt, y_pos_batt - 25);
	for (uint8_t i = 1; i < standart_helligkeit; i++)
	{
		DelayMili(10);
		ILI9341_SetBrightness(i);
	}
	DelayMili(delay_menus);
}
//-------------------------------------------------------------------
void drw_menu_segment(const char *text, const uint8_t *bitmap_right, const uint8_t *bitmap_mid, const uint8_t *bitmap_left)
{
	const uint16_t color_block = COLOR_MENU_BLOCK;
	const uint16_t color_curr_block = COLOR_WHITE;
	const uint16_t y_block_left_right = 100;
	const uint16_t y_block_mid = y_block_left_right - 10;

	const uint16_t x_block_left = 33;
	const uint16_t x_block_mid = x_block_left + 86;
	const uint16_t x_block_right = x_block_mid + 96;

	const uint16_t x_text = 86;
	const uint16_t y_text = 55;
	ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG);
	ILI9341_printText(text, x_text + 25, y_text, COLOR_WHITE,
					  COLOR_MENU_BG, 2);
	ILI9341_drawBitmapFast(x_block_left, y_block_left_right, bitmap_left,
						   66, 66, color_block, sizeof bitmap_left);
	ILI9341_drawBitmapFast(x_block_mid, y_block_mid, bitmap_mid, 80, 80,
						   color_curr_block, sizeof bitmap_mid);
	ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
						   bitmap_right, 66, 66, color_block, sizeof bitmap_right);
}
void drw_menu_pic(const uint8_t current_page)
{
	switch (current_page)
	{
	case SENSOR_MENU:
		drw_menu_segment("   SENSORS  ", info_66x66, sensors_80x80, pt100_66x66);
		break;
	case SENSOR_MENU_PT100:
		drw_menu_segment("   PT100    ", sensors_66x66, pt100_80x80, actor_010V_66x66);
		break;
	case ACTOR_MENU_010V:
		drw_menu_segment("ACTORS 0-10V", pt100_66x66, actor_010V_80x80, actor_oc_66x66);
		break;
	case ACTOR_MENU_OC:
		drw_menu_segment("  ACTORS OC ", actor_010V_66x66, actor_oc_80x80, settings_66x66);
		break;
	case SETTINGS_MENU:
		drw_menu_segment("  SETTINGS  ", actor_oc_66x66, settings_80x80, info_66x66);
		break;
	case INFO_MENU:
		drw_menu_segment("    INFO    ", settings_66x66, info_80x80, sensors_66x66);
		break;
	}
}
