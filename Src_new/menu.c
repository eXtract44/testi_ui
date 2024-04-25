#include "menu.h"

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
uint16_t valOld = 0;
uint16_t delay_menus = 500;

#define text_menu_size &Font16
#define menu_bar_up 0, 0, 319, 39, COLOR_MENU_BAR_DOWN
#define menu_bar_down 0, 200, 319, 239, COLOR_MENU_BAR_DOWN

//-------------------------------------------------------------------

/*******************************/
#define i16 uint16_t
#define u16 uint16_t
/*******************************/
uint8_t drw_btn_raw(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t frame_size, uint16_t color)
{
	uint8_t passed = 1;
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
	for (uint8_t i = 0; i < frame_size; i++)
	{
		ILI9341_drawRect(frame_size / 2 + x - i, y - i, frame_size / 2 + x + w + i, y + h + i, color);
	}
	return passed;
}
void drw_btn_char(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t frame_size, uint16_t color,char* text){
	drw_btn_raw(x, y, w, h,frame_size, color);
	ILI9341_printText(text, x + 22, y + 8, color, bg, 1);
}
void draw_button(uint16_t x, uint16_t y, uint16_t color, uint16_t bg, char *c)
{
	for (uint8_t i = 0; i <= 2; i++)
	{
		ILI9341_drawRect(x - i, y - i, x + 53 + i, y + 22 + i, color);
	}
	ILI9341_printText(c, x + 22, y + 8, color, bg, 1);
}
//-------------------------------------------------------------------
void draw_button_helper(uint8_t c)
{
	uint16_t x_left_tri = x_pos_button_l + 65;
	uint16_t width_tri = 14;
	uint16_t x_right_tri = x_pos_button_r - 12 - width_tri;
	uint16_t hight_tri = width_tri - 2;
	uint16_t y_tri = y_pos_button_l - 25;

	switch (c)
	{
	case 'l':
		ILI9341_drawTriangle(x_left_tri, y_tri - hight_tri,
							 x_left_tri + width_tri, y_tri - hight_tri,
							 x_left_tri + (width_tri / 2), y_tri,
							 COLOR_WHITE);
		ILI9341_drawCircle(x_left_tri + (width_tri / 2),
						   y_tri - (hight_tri / 2), 12, COLOR_WHITE);
		break;
	case 'r':
		ILI9341_drawTriangle(x_right_tri, y_tri, x_right_tri + width_tri, y_tri,
							 x_right_tri + (width_tri / 2), y_tri - hight_tri,
							 COLOR_WHITE);
		ILI9341_drawCircle(x_right_tri + (width_tri / 2),
						   y_tri - (hight_tri / 2), 12, COLOR_WHITE);
		break;
	}
	draw_button(x_pos_button_l, y_pos_button_l - 43, COLOR_WHITE,
				COLOR_MENU_BAR_DOWN, "/10");
	draw_button(x_pos_button_r, y_pos_button_r - 43, COLOR_WHITE,
				COLOR_MENU_BAR_DOWN, "*10");
}
//-------------------------------------------------------------------
void draw_m_frame(uint16_t x, uint16_t y)
{
	for (uint16_t i = 0; i <= 2; i++)
	{
		ILI9341_drawRect(x - i, y - i, x + 96 + i, y + 23 + i, COLOR_WHITE);
	} // x len=86px y len=23
}
void draw_s_frame(uint16_t x, uint16_t y)
{
	for (uint16_t i = 0; i <= 2; i++)
	{
		ILI9341_drawRect(x - i, y - i, x + 96 + i, y + 23 + i, COLOR_ORANGE);
	} // x len=86px y len=23
}
void draw_start_button(uint16_t color)
{
	ILI9341_fillCircle(x_pos_button_c + 10, y_pos_button_c + 10, 5, color);
	for (uint8_t i = 0; i < 3; i++)
		ILI9341_drawCircle(x_pos_button_c + 10, y_pos_button_c + 10, 13 + i,
						   color);
}
//-------------------------------------------------------------------
void draw_legende(uint16_t y, uint16_t cur_page)
{
	uint16_t x = 16;
	uint16_t color_bar = COLOR_MENU_BAR_DOWN;
	// uint16_t color_text = COLOR_WHITE;
	uint16_t color_text1 = COLOR_YELLOW;
	uint16_t color_text_bg = COLOR_MENU_BAR_DOWN;
	uint16_t size = 38; // 43
	uint16_t high = 22;
	//	uint16_t anim_del = 200;
	//	uint8_t anim_cy_old = 0;
	ILI9341_fillRect(0, y, 319, y + high, color_bar);
	ILI9341_drawLine(0, y, 319, y, COLOR_WHITE);
	ILI9341_drawLine(0, y + high, 319, y + high, COLOR_WHITE);
	for (uint16_t i = 0; i <= 310; i += size)
	{
		ILI9341_drawLine(x - 10 + i, y, x - 10 + i, y + high, COLOR_WHITE);
	}

	switch (cur_page)
	{
	case 1: // 0-10v
			//		for (uint8_t i = 0; i <= anim_cy_old; i++) {
		ILI9341_printText("   ", x, y + 7, color_text1, color_text_bg, 1);
		ILI9341_printText("   ", x + size, y + 7, color_text1, color_text_bg,
						  1);
		ILI9341_printText("   ", x + size * 2, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 3, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("IN1", x + size * 4, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText(" Y ", x + size * 5, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("GND", x + size * 6, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("24V", x + size * 7, y + 7, color_text1,
						  color_text_bg, 1);

		//			DelayMili(anim_del);
		//			ILI9341_printText("   ", x, y + 7, color_text, color_text_bg, 1);
		//			ILI9341_printText("   ", x + size, y + 7, color_text, color_text_bg,
		//					1);
		//			ILI9341_printText("   ", x + size * 2, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 3, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("IN1", x + size * 4, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText(" Y ", x + size * 5, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("GND", x + size * 6, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("24V", x + size * 7, y + 7, color_text,
		//					color_text_bg, 1);
		//			DelayMili(anim_del);
		//		}
		break;
	case 2: // auf/zu
			//		for (uint8_t i = 0; i <= anim_cy_old; i++) {
		ILI9341_printText("  ", x, y + 7, color_text1, color_text_bg, 1);
		ILI9341_printText("  ", x + size, y + 7, color_text1, color_text_bg, 1);
		ILI9341_printText("  ", x + size * 2, y + 7, color_text1, color_text_bg,
						  1);
		ILI9341_printText(" ZU", x + size * 3, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("AUF", x + size * 4, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText(" + ", x + size * 5, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("GND", x + size * 6, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("24V", x + size * 7, y + 7, color_text1,
						  color_text_bg, 1);

		//			DelayMili(anim_del);
		//			ILI9341_printText("   ", x, y + 7, color_text, color_text_bg, 1);
		//			ILI9341_printText("   ", x + size, y + 7, color_text, color_text_bg,
		//					1);
		//			ILI9341_printText("ZU ", x + size * 2, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText(" + ", x + size * 3, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("AUF", x + size * 4, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText(" + ", x + size * 5, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("GND", x + size * 6, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("24V", x + size * 7, y + 7, color_text,
		//					color_text_bg, 1);
		//			DelayMili(anim_del);
		//		}
		break;
	case 3: // in1
			//		for (uint8_t i = 0; i <= anim_cy_old; i++) {
		ILI9341_printText("   ", x, y + 7, color_text1, color_text_bg, 1);
		ILI9341_printText("   ", x + size, y + 7, color_text1, color_text_bg,
						  1);
		ILI9341_printText("   ", x + size * 2, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 3, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("IN1", x + size * 4, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 5, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("GND", x + size * 6, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("24V", x + size * 7, y + 7, color_text1,
						  color_text_bg, 1);

		//			DelayMili(anim_del);
		//			ILI9341_printText("   ", x, y + 7, color_text, color_text_bg, 1);
		//			ILI9341_printText("   ", x + size, y + 7, color_text, color_text_bg,
		//					1);
		//			ILI9341_printText("   ", x + size * 2, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 3, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("IN1", x + size * 4, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 5, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("GND", x + size * 6, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("24V", x + size * 7, y + 7, color_text,
		//					color_text_bg, 1);
		//			DelayMili(anim_del);
		//		}
		break;

	case 4: // in1 in2
			//		for (uint8_t i = 0; i <= anim_cy_old; i++) {
		ILI9341_printText("   ", x, y + 7, color_text1, color_text_bg, 1);
		ILI9341_printText("   ", x + size, y + 7, color_text1, color_text_bg,
						  1);
		ILI9341_printText("   ", x + size * 2, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("IN2", x + size * 3, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("IN1", x + size * 4, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 5, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("GND", x + size * 6, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("24V", x + size * 7, y + 7, color_text1,
						  color_text_bg, 1);

		//			DelayMili(anim_del);
		//			ILI9341_printText("   ", x, y + 7, color_text, color_text_bg, 1);
		//			ILI9341_printText("   ", x + size, y + 7, color_text, color_text_bg,
		//					1);
		//			ILI9341_printText("   ", x + size * 2, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("IN2", x + size * 3, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("IN1", x + size * 4, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 5, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("GND", x + size * 6, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("24V", x + size * 7, y + 7, color_text,
		//					color_text_bg, 1);
		//			DelayMili(anim_del);
		//		}
		break;
	case 5: // regler passiv
			//		for (uint8_t i = 0; i <= anim_cy_old; i++) {
		ILI9341_printText("PT+", x, y + 7, color_text1, color_text_bg, 1);
		ILI9341_printText("PT-", x + size, y + 7, color_text1, color_text_bg,
						  1);
		ILI9341_printText("GND", x + size * 2, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 3, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("IN1", x + size * 4, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText(" Y ", x + size * 5, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 6, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 7, y + 7, color_text1,
						  color_text_bg, 1);

		//			DelayMili(anim_del);
		//			ILI9341_printText("PT+", x, y + 7, color_text, color_text_bg, 1);
		//			ILI9341_printText("PT-", x + size, y + 7, color_text, color_text_bg,
		//					1);
		//			ILI9341_printText("   ", x + size * 2, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 3, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("IN1", x + size * 4, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText(" Y ", x + size * 5, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("GND", x + size * 6, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 7, y + 7, color_text,
		//					color_text_bg, 1);
		//			DelayMili(anim_del);
		//		}
		break;
	case 6: // pt100
			//		for (uint8_t i = 0; i <= anim_cy_old; i++) {
		ILI9341_printText("PT+", x, y + 7, color_text1, color_text_bg, 1);
		ILI9341_printText("PT-", x + size, y + 7, color_text1, color_text_bg,
						  1);
		ILI9341_printText("GND", x + size * 2, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 3, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 4, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 5, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 6, y + 7, color_text1,
						  color_text_bg, 1);
		ILI9341_printText("   ", x + size * 7, y + 7, color_text1,
						  color_text_bg, 1);

		//			DelayMili(anim_del);
		//			ILI9341_printText("PT+", x, y + 7, color_text, color_text_bg, 1);
		//			ILI9341_printText("PT-", x + size, y + 7, color_text, color_text_bg,
		//					1);
		//			ILI9341_printText("   ", x + size * 2, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 3, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 4, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 5, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("GND", x + size * 6, y + 7, color_text,
		//					color_text_bg, 1);
		//			ILI9341_printText("   ", x + size * 7, y + 7, color_text,
		//					color_text_bg, 1);
		//			DelayMili(anim_del);
		//		}
		break;
	}
}
//-------------------------------------------------------------------
void draw_menu_home()
{
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_RESET); // 24VINTERN
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_RESET); // 24VOUT
	ILI9341_SetBrightness(1);
	ILI9341_fill(COLOR_MENU_BG);
	// top bar
	ILI9341_fillRect(menu_bar_up);
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 39 + i, 319, 39 + i, COLOR_WHITE);
	// down bar
	ILI9341_fillRect(menu_bar_down);
	// Triangle BUTTOM + lines
	for (uint8_t i = 0; i <= 2; i++)
		ILI9341_drawLine(0, 198 + i, 319, 198 + i, COLOR_WHITE);
	for (uint8_t i = 0; i <= 3; i++)
		ILI9341_drawLine(140 - i, 200, 160 - i, 185, COLOR_WHITE);
	for (uint8_t i = 0; i <= 3; i++)
		ILI9341_drawLine(160 + i, 185, 180 + i, 200, COLOR_WHITE);
	ILI9341_fillTriangle(140, 200, 180, 200, 160, 185, COLOR_MENU_BAR_DOWN);

	select_main_menu(page_main_menu);
	draw_start_button(COLOR_WHITE);

	read_adc_battery(x_pos_batt, y_pos_batt - 25);
	ILI9341_drawBitmapFastBar(15, 10, mcu_20x20, 20, 20, COLOR_YELLOW,
							  sizeof mcu_20x20);

	for (uint8_t i = 1; i < standart_helligkeit; i++)
	{
		DelayMili(10);
		ILI9341_SetBrightness(i);
	}
	DelayMili(delay_menus);
}
//-------------------------------------------------------------------
void draw_menu_antrieb_r()
{
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
	ILI9341_string(frame_val_xtext, frame_val_ytext, " Y=", COLOR_WHITE,
				   COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize, "IN1",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, antrieb_56x112, 56, 112, COLOR_WHITE,
						   sizeof antrieb_56x112);
	// ILI9341_drawBitmapFast(170, 80, anim_kl0_57x138, 57, 138, COLOR_WHITE, sizeof anim_kl0_57x138,sizeof);
	draw_legende(3, 1);
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

	for (uint8_t i = 1; i < standart_helligkeit; i++)
	{
		DelayMili(10);
		ILI9341_SetBrightness(i);
	}
	DelayMili(delay_menus);
}
//-------------------------------------------------------------------
// void draw_menu_antrieb_s_info() { //Antriebtester S
//	//ILI9341_SetBrightness(1);
//	ILI9341_fill(COLOR_MENU_BG);
//	ILI9341_fillRect(menu_bar_down);
//	ILI9341_setFont(text_menu_size);
//	draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize * 2);
// uint16_t xsta = 140;
// uint16_t ylen = 100;
// uint16_t xlen = 38;
//	ILI9341_drawLine(xsta, 20, xsta, ylen,COLOR_WHITE);
//	ILI9341_drawLine(xsta+xlen, 20, xsta+xlen, ylen,COLOR_WHITE);
//	ILI9341_drawLine(xsta+xlen*2, 20, xsta+xlen*2, ylen,COLOR_WHITE);
//
//
//
//
//	draw_legende(3, 2);
//	draw_start_button(COLOR_WHITE);
////	for (uint8_t i = 1; i < 101; i++) {
////		DelayMili(10);
////		ILI9341_SetBrightness(i);
////	}
//	DelayMili(delay_menus);
//}
//-------------------------------------------------------------------
void draw_menu_raumregler_aktiv()
{ // sensor menu
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
	draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize * 3);
	//*** Draw text for frames
	ILI9341_string(frame_val_xtext, frame_val_ytext, "IN1", COLOR_WHITE,
				   COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize,
				   "<       >",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "IN2", COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 3,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, raumregler_71x71, 71, 71, COLOR_WHITE,
						   sizeof raumregler_71x71);
	draw_legende(3, 4);
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
void draw_menu_raumregler_passiv()
{														// sensor menu
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
	//*** Draw text for frames
	ILI9341_string(frame_val_xtext, frame_val_ytext, "POT", COLOR_WHITE,
				   COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize, "TEM",
				   COLOR_WHITE, COLOR_MENU_BG);

	// ILI9341_drawBitmapFast(45, 80, raumregler_71x71, 71, 71, COLOR_WHITE,sizeof raumregler_71x71);
	// draw_pt100(350, 45, COLOR_WHITE);
	// draw_poti(90 + 43, 45, COLOR_WHITE);
	//	ILI9341_printText("1K Ohm", mess_val_xstart - 80, adc_val_ystart,
	//			COLOR_RED, COLOR_MENU_BG, 1);

	draw_pt100(50, 134, COLOR_YELLOW);
	draw_poti(195, 134, COLOR_YELLOW);
	draw_start_button(COLOR_WHITE);
	draw_legende(3, 5);
	read_adc_battery(x_pos_batt, y_pos_batt);

	for (uint8_t i = 1; i < standart_helligkeit; i++)
	{
		DelayMili(10);
		ILI9341_SetBrightness(i);
	}
	DelayMili(delay_menus);
}
//-------------------------------------------------------------------
/*void draw_menu_raumtemp() { //sensor menu1
 //HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_SET); //24VINTERN
 HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_SET); //24VOUT
 ILI9341_SetBrightness(1);
 ILI9341_fill(COLOR_MENU_BG);
 ILI9341_fillRect(menu_bar_down);
 ILI9341_setFont(text_menu_size);
 // Draw frames for meas values
 draw_m_frame(frame_val_xstart, frame_val_ystart);
 draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize);
 draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize * 2);
 // Draw text for frames
 ILI9341_string(frame_val_xtext, frame_val_ytext, "IN1", COLOR_WHITE,
 COLOR_MENU_BG);
 ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize,
 "<       >",
 COLOR_WHITE, COLOR_MENU_BG);
 ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
 "Ub=", COLOR_WHITE, COLOR_MENU_BG);

 ILI9341_drawBitmapFast(10, 50, raumtemp_71x71, 71, 71, COLOR_WHITE,
 sizeof raumtemp_71x71);
 draw_legende(3, 3);
 draw_start_button(COLOR_WHITE);
 read_adc_battery(x_pos_batt, y_pos_batt);

 for (uint8_t i = 1; i < 101; i++) {
 DelayMili(10);
 ILI9341_SetBrightness(i);
 }
 DelayMili(delay_menus);
 }
 */
//-------------------------------------------------------------------
void draw_menu_kanaltemp()
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
	ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize,
				   "<       >",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, kanaltemp_71x112, 71, 112, COLOR_WHITE,
						   sizeof kanaltemp_71x112);
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
void draw_menu_feuchte()
{
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
	draw_m_frame(frame_val_xstart, frame_val_ystart + frame_val_ysize * 3);
	//*** Draw text for frames
	ILI9341_string(frame_val_xtext, frame_val_ytext, "IN1", COLOR_WHITE,
				   COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize, "IN2",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize * 2,
				   "<       >",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 3,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, temp_feuchte_71x112, 71, 112, COLOR_WHITE,
						   sizeof temp_feuchte_71x112);
	draw_legende(3, 4);
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
void draw_menu_druckluft()
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
	ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize,
				   "<       >",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, druckluft_71x71, 71, 71, COLOR_WHITE,
						   sizeof druckluft_71x71);
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
void draw_menu_druckwasser()
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
	ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize,
				   "<       >",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, druckwasser_71x112, 71, 112, COLOR_WHITE,
						   sizeof druckwasser_71x112);
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
void draw_menu_luftquali()
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
	ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize,
				   "<       >",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, luftquali_71x112, 71, 112, COLOR_WHITE,
						   sizeof luftquali_71x112);
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
void draw_menu_helligkeit()
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
	ILI9341_string(frame_val_xtext_m, frame_val_ytext + frame_val_ysize,
				   "<       >",
				   COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_string(frame_val_xtext, frame_val_ytext + frame_val_ysize * 2,
				   "Ub=", COLOR_WHITE, COLOR_MENU_BG);

	ILI9341_drawBitmapFast(10, 50, helligkeit_71x112, 71, 112, COLOR_WHITE,
						   sizeof helligkeit_71x112);
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
void draw_menu_ysensor()
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
void draw_premenu_ysensor()
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
//-------------------------------------------------------------------
void draw_graphic_m(uint16_t x, uint16_t y, uint16_t color)
{

	uint8_t l = 5;
	uint8_t h = 160;
	uint8_t w = 200;
	uint8_t s = 20;
	ILI9341_fillRect(x, y + 7, x + w + 5, y + h + 10, COLOR_MENU_BG);
	ILI9341_drawLine(x, y + 20, x, y + h, color); // 200 px len max
	for (uint8_t i = 0; i <= w; i += 20)
	{
		ILI9341_drawLine(x + i, y + 20, x + i, y + h, COLOR_LGRAY);
	}
	ILI9341_drawLine(x, y + h, x + w, y + h, color);
	for (uint8_t i = 0; i <= h - 20; i += 20)
	{
		ILI9341_drawLine(x, y + h - i, x + w, y + h - i, COLOR_LGRAY);
	}

	for (uint8_t i = 20; i <= h; i += s)
	{
		ILI9341_drawLine(x - l, y + i, x + l, y + i, color);
	}
	for (uint8_t i = 0; i <= w; i += s)
	{
		ILI9341_drawLine(x + i, y - l + h, x + i, y + l + h, color);
	}
	ILI9341_setFont(&Font12);

	ILI9341_string(x - 25, y - 5 + s, "120", color, COLOR_MENU_BG);
	ILI9341_string(x - 25, y - 5 + s * 2, "100", color, COLOR_MENU_BG);
	ILI9341_string(x - 25, y - 5 + s * 3, "80", color, COLOR_MENU_BG);
	ILI9341_string(x - 25, y - 5 + s * 4, "60", color, COLOR_MENU_BG);
	ILI9341_string(x - 25, y - 5 + s * 5, "40", color, COLOR_MENU_BG);
	ILI9341_string(x - 25, y - 5 + s * 6, "20", color, COLOR_MENU_BG);
	ILI9341_string(x - 25, y - 5 + s * 7, "0*c", color, COLOR_MENU_BG);
	ILI9341_string(x - 25, y - 5 + s * 8, "-20", color, COLOR_MENU_BG);

	ILI9341_string(x - 5 + s, y + h + 2, "1", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 2 - 5, y + h + 2, "2", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 3 - 5, y + h + 2, "3", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 4 - 5, y + h + 2, "4", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 5 - 5, y + h + 2, "5", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 6 - 5, y + h + 2, "6", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 7 - 5, y + h + 2, "7", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 8 - 5, y + h + 2, "8", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 9 - 5, y + h + 2, "9", color, COLOR_MENU_BG);
	ILI9341_string(x + s * 10 - 5, y + h + 2, "10 MIN", color, COLOR_MENU_BG);
	// ILI9341_setFont(&Font16);
	// ILI9341_string(x + s * 8 - 5, y + h + 5, "t.min", color, COLOR_MENU_BG);
	// ILI9341_string(x - 5, y, "*C", color, COLOR_MENU_BG);
}
//-------------------------------------------------------------------
// void draw_graphic_h(uint16_t x, uint16_t y, uint16_t color) {
//	uint8_t l = 5;
//	uint8_t h = 160;
//	uint8_t w = 200;
//	uint8_t s = 20;
//	ILI9341_drawLine(x, y + 20, x, y + h, color);   //200 px len max
//	for (uint8_t i = 0; i <= w; i += 20) {
//		ILI9341_drawLine(x + i, y + 20, x + i, y + h, COLOR_LGRAY);
//	}
//	ILI9341_drawLine(x, y + h, x + w, y + h, color);
//	for (uint8_t i = 0; i <= h - 20; i += 20) {
//		ILI9341_drawLine(x, y + h - i, x + w, y + h - i, COLOR_LGRAY);
//	}
//
//	for (uint8_t i = 0; i <= h; i += s) {
//		ILI9341_drawLine(x - l, y + i, x + l, y + i, color);
//	}
//	for (uint8_t i = 0; i <= w; i += s) {
//		ILI9341_drawLine(x + i, y - l + h, x + i, y + l + h, color);
//	}
//	ILI9341_setFont(&Font12);
//
//	ILI9341_string(x - 25, y - 5 + s, "120", color, COLOR_MENU_BG);
//	ILI9341_string(x - 25, y - 5 + s * 2, "100", color, COLOR_MENU_BG);
//	ILI9341_string(x - 25, y - 5 + s * 3, "80", color, COLOR_MENU_BG);
//	ILI9341_string(x - 25, y - 5 + s * 4, "60", color, COLOR_MENU_BG);
//	ILI9341_string(x - 25, y - 5 + s * 5, "40", color, COLOR_MENU_BG);
//	ILI9341_string(x - 25, y - 5 + s * 6, "20", color, COLOR_MENU_BG);
//	ILI9341_string(x - 25, y - 5 + s * 7, " 0", color, COLOR_MENU_BG);
//	ILI9341_string(x - 25, y - 5 + s * 8, "-20", color, COLOR_MENU_BG);
//
//	ILI9341_string(x - 5 + s, y + h + 5, "1", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 2 - 5, y + h + 5, "2", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 3 - 5, y + h + 5, "3", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 4 - 5, y + h + 5, "4", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 5 - 5, y + h + 5, "5", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 6 - 5, y + h + 5, "6", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 7 - 5, y + h + 5, "7", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 8 - 5, y + h + 5, "8", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 9 - 5, y + h + 5, "9", color, COLOR_MENU_BG);
//	ILI9341_string(x + s * 10 - 5, y + h + 5, "10 std", color, COLOR_MENU_BG);
//}
//-------------------------------------------------------------------
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
	//	uint8_t buf[318];
	//	uint16_t x, y;
	//
	//	ILI9341_fill(COLOR_MENU_BG);
	//	/*
	//	 // Draw sin-, cos- and tan-lines
	//	 for (int i = 1; i < 478; i++) {
	//	 ILI9341_drawPixel(i, 159 + (sin(((i * 1.13) * 3.14) / 180)) * 95,
	//	 COLOR_BLACK);
	//	 }
	//
	//	 for (int i = 1; i < 478; i++) {
	//	 ILI9341_drawPixel(i, 159 + cos(((i * 1.13) * 3.14) / 180) * 95,
	//	 COLOR_BLACK);
	//	 }
	//
	//	 for (int i = 1; i < 478; i++) {
	//	 ILI9341_drawPixel(i, 159 + (tan(((i * 1.13) * 3.14) / 180)),
	//	 COLOR_BLACK);
	//	 }
	//	 */
	//	//HAL_Delay(2000);
	//	// Draw a moving sinewave
	//	x = 1;
	//	for (uint16_t i = 1; i < (318 * 15); i++) {
	//		x++;
	//		if (x == 319)
	//			x = 1;
	//		if (i > 319) {
	//			if ((x == 239) || (buf[x - 1] == 159))
	//				//else
	//				ILI9341_drawPixel(x, buf[x - 1], COLOR_WHITE);
	//		}
	//
	//		//
	//		//DelayMili(1500);
	//		y = 159 + (sin(((i * 0.7) * 3.14) / 180) * (90 - (i / 100)));
	//		ILI9341_drawPixel(x, y, COLOR_WHITE);
	//		buf[x - 1] = y;
	//	}
	//
	//	ILI9341_setFont(&Font24);
	//	ILI9341_string(80, 120, "TESTI V0.1", COLOR_WHITE, COLOR_MENU_BG);
	//	HAL_Delay(2500);
	//---

	ILI9341_fill(COLOR_MENU_BG);
	//	for (uint8_t z = 0; z < 12; z++) {
	//		ILI9341_drawBitmapFast(rand() % 219, rand() % 160, bruno_50x50, 50,
	//				50, COLOR_WHITE, sizeof bruno_50x50);
	//		HAL_Delay(100);
	//	}
	ILI9341_setFont(&Font24);
	// ILI9341_string(180, 150, "TESTI V0.1", COLOR_WHITE, COLOR_MENU_BG);
	// HAL_Delay(delay_menus);
	ILI9341_string(80, 120, "TESTI V0.2", COLOR_WHITE, COLOR_MENU_BG);
	for (uint16_t i = 0; i < 800; i++)
	{
		for (uint16_t j = 0; j < 200; j++)
		{
			ILI9341_drawPixel(rand() % ILI9341_WIDTH, rand() % ILI9341_HEIGHT,
							  COLOR_MENU_BG);
		}
		ILI9341_drawPixel(rand() % ILI9341_WIDTH, rand() % ILI9341_HEIGHT,
						  ILI9341_RandColor());
		delaymic(100);
	}
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
void draw_infoMenu2()
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

	ILI9341_setFont(&Font16);
	//	ILI9341_string(15, 40, "MCU: STM32F446VET6 180MHz, 512Kb Flash",
	//	COLOR_WHITE, COLOR_MENU_BG);
	ILI9341_setFont(text_menu_size);
	ILI9341_string(15, 70, "designed and made by:", COLOR_WHITE,
				   COLOR_MENU_BG);
	ILI9341_string(15, 90, "Alexander Onopriienko", COLOR_RED, COLOR_MENU_BG);

	// ILI9341_string(15, 140, "for R. Plett", COLOR_RED, COLOR_MENU_BG);
	ILI9341_drawBitmapFast(220, 130, logo_stm32_50x50, 50, 50, 0x061F,
						   sizeof logo_stm32_50x50);
	// draw_button(x_pos_button_l,y_pos_button_l,COLOR_WHITE,COLOR_MENU_BAR_DOWN,"<<");
	// draw_button(x_pos_button_r,y_pos_button_r,COLOR_WHITE,COLOR_MENU_BAR_DOWN,">>");

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
void draw_infoMenuUp()
{
	// ILI9341_SetBrightness(1);
	HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_SET); // 24VINTERN
	HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_SET); // 24VOUT
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
	uint16_t yStart = 50;
	uint16_t ySize = 21;

	ILI9341_string(xStart, yStart, "PA3 IN3 RANK1 PT100", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize, "PC0 IN10 RANK2 24V", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 2, "PC1 IN11 RANK3 Y", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 3, "PC2 IN12 RANK4 IN1", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 4, "PC3 IN13 RANK5 IN2", color_text,
				   color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 5, "INTemp. RANK6 MCU.TEMP",
				   color_text, color_text_bg);
	ILI9341_string(xStart, yStart + ySize * 6, "PA2 IN2 RANK7 BATTERY", color_text,
				   color_text_bg);

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
void select_main_menu(uint8_t menu_page)
{ // draw_select_menu
	uint16_t color_block = COLOR_MENU_BLOCK;
	uint16_t color_curr_block = COLOR_WHITE;
	uint16_t y_block_left_right = 100;
	uint16_t y_block_mid = y_block_left_right - 10;

	uint16_t x_block_left = 33;
	uint16_t x_block_mid = x_block_left + 86;
	uint16_t x_block_right = x_block_mid + 96;

	uint16_t x_text = 86;
	uint16_t y_text = 55;

	//	uint16_t color_bar = COLOR_WHITE;
	//	uint16_t color_curr_bar = 0x7A7D;
	//	uint16_t x_bar = 80;
	//	uint16_t y_bar = 10;

	switch (menu_page)
	{
	case 1:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar-5, y_bar, menubar1_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("SENSOREN", x_text + 25, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right, ysensor_66x66,
							   66, 66, color_block, sizeof ysensor_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, sensoren_80x80, 80, 80,
							   color_curr_block, sizeof sensoren_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   antrieb_r_66x66, 66, 66, color_block, sizeof antrieb_r_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 2:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*2-29, y_bar, menubar2_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("ANTRIEBE/VENTILE", x_text - 20, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right, sensoren_66x66,
							   66, 66, color_block, sizeof sensoren_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, antrieb_r_80x80, 80,
							   80, color_curr_block, sizeof antrieb_r_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right, info_66x66,
							   66, 66, color_block, sizeof info_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 3:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*2+26, y_bar, menubar3_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("INFO", x_text + 50, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   antrieb_r_66x66, 66, 66, color_block, sizeof antrieb_r_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, info_80x80, 80, 80,
							   color_curr_block, sizeof info_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right, ysensor_66x66,
							   66, 66, color_block, sizeof ysensor_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 4:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*2+26, y_bar, menubar3_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("EINSTELLUNG", x_text + 10, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right, info_66x66, 66,
							   66, color_block, sizeof info_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, ysensor_80x80, 80, 80,
							   color_curr_block, sizeof info_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   sensoren_66x66, 66, 66, color_block, sizeof sensoren_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	}
}
//-------------------------------------------------------------------
void select_sensor_submenu(uint8_t menu_page)
{ // draw_select_menu
	uint16_t color_block = COLOR_MENU_BLOCK;
	uint16_t color_curr_block = COLOR_WHITE;
	uint16_t y_block_left_right = 100;
	uint16_t y_block_mid = y_block_left_right - 10;

	uint16_t x_block_left = 33;
	uint16_t x_block_mid = x_block_left + 86;
	uint16_t x_block_right = x_block_mid + 96;

	uint16_t x_text = 86;
	uint16_t y_text = 55;

	//	uint16_t color_bar = COLOR_WHITE;
	//	uint16_t color_curr_bar = 0x7A7D;
	//	uint16_t x_bar = 80;
	//	uint16_t y_bar = 10;
	// uint16_t k_bar = 20;

	switch (menu_page)
	{
	case 1:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar-5, y_bar, menubar1_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("RAUMREGLER AKTIV", x_text - 25, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   home_menu_66x66, 66, 66, color_block, sizeof home_menu_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, raumregler_80x80, 80,
							   80, color_curr_block, sizeof raumregler_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   raumregler_66x66, 66, 66, color_block, sizeof raumregler_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 2:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar-5, y_bar, menubar1_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("RAUMREGLER PASSIV", x_text - 25, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   raumregler_66x66, 66, 66, color_block, sizeof raumregler_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, raumregler_80x80, 80,
							   80, color_curr_block, sizeof raumregler_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right, temp_66x66,
							   66, 66, color_block, sizeof temp_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
		//	case 3: //
		//		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		//		//ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		//		//ILI9341_drawBitmapFast(x_bar*2-29, y_bar, menubar2_80x34, 80, 34, color_curr_bar);
		//		ILI9341_printText("RAUMTEMPERATUR", x_text - 10, y_text, COLOR_WHITE,
		//		COLOR_MENU_BG, 2);
		//
		//		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
		//				raumregler_66x66, 66, 66, color_block, sizeof raumregler_66x66);
		//		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, temp_80x80, 80, 80,
		//				color_curr_block, sizeof temp_80x80);
		//		ILI9341_drawBitmapFast(x_block_right, y_block_left_right, temp_66x66,
		//				66, 66, color_block, sizeof temp_66x66);
		//		//HAL_Delay(delay_menus);
		//		//ILI9341_Delay(del_menu);
		//		break;
	case 3:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*2+26, y_bar, menubar3_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("TEMPERATUR FUEHLER", x_text - 35, y_text,
						  COLOR_WHITE, COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   raumregler_66x66, 66, 66, color_block, sizeof raumregler_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, temp_80x80, 80, 80,
							   color_curr_block, sizeof temp_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right, feuchte_66x66,
							   66, 66, color_block, sizeof feuchte_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 4:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		/// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+8, y_bar, menubar4_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("FEUCHTE", x_text + 35, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right, temp_66x66, 66,
							   66, color_block, sizeof temp_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, feuchte_80x80, 80, 80,
							   color_curr_block, sizeof feuchte_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   druckwasser_66x66, 66, 66, color_block,
							   sizeof druckwasser_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 5:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+74, y_bar, menubar5_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("DRUCKFLUESSIGKEIT", x_text - 25, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right, feuchte_66x66,
							   66, 66, color_block, sizeof feuchte_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, druckwasser_80x80, 80,
							   80, color_curr_block, sizeof druckwasser_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   druckluft_66x66, 66, 66, color_block, sizeof druckluft_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 6:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+74, y_bar, menubar5_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("DRUCKLUFT", x_text + 20, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   druckwasser_66x66, 66, 66, color_block,
							   sizeof druckwasser_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, druckluft_80x80, 80,
							   80, color_curr_block, sizeof druckluft_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   luftquali_66x66, 66, 66, color_block, sizeof luftquali_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 7:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+74, y_bar, menubar5_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("LUFTQUALITAET", x_text - 10, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   druckluft_66x66, 66, 66, color_block, sizeof druckluft_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, luftquali_80x80, 80,
							   80, color_curr_block, sizeof luftquali_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   helligkeit_66x66, 66, 66, color_block, sizeof helligkeit_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 8:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+74, y_bar, menubar5_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("HELLIGKEIT", x_text + 10, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   luftquali_66x66, 66, 66, color_block, sizeof luftquali_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, helligkeit_80x80, 80,
							   80, color_curr_block, sizeof helligkeit_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right, pt100_66x66,
							   66, 66, color_block, sizeof pt100_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 9:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+74, y_bar, menubar5_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("PT100 TESTER", x_text + 5, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   helligkeit_66x66, 66, 66, color_block, sizeof helligkeit_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, pt100_80x80, 80, 80,
							   color_curr_block, sizeof pt100_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right, ysensor_66x66,
							   66, 66, color_block, sizeof ysensor_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 10:														 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+74, y_bar, menubar5_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("DEIN SENSOR", x_text + 10, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right, pt100_66x66,
							   66, 66, color_block, sizeof pt100_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, ysensor_80x80, 80, 80,
							   color_curr_block, sizeof ysensor_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   home_menu_66x66, 66, 66, color_block, sizeof home_menu_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	case 11:														 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*3+74, y_bar, menubar5_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("HOME", x_text + 50, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right, ysensor_66x66,
							   66, 66, color_block, sizeof ysensor_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, home_menu_80x80, 80,
							   80, color_curr_block, sizeof home_menu_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   raumregler_66x66, 66, 66, color_block, sizeof raumregler_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	}
}
//-------------------------------------------------------------------
void select_antrieb_submenu(uint8_t menu_page)
{ // draw_select_menu
	uint16_t color_block = COLOR_MENU_BLOCK;
	uint16_t color_curr_block = COLOR_WHITE;
	uint16_t y_block_left_right = 100;
	uint16_t y_block_mid = y_block_left_right - 10;

	uint16_t x_block_left = 33;
	uint16_t x_block_mid = x_block_left + 86;
	uint16_t x_block_right = x_block_mid + 96;

	uint16_t x_text = 86;
	uint16_t y_text = 55;

	// uint16_t color_bar = COLOR_WHITE;
	// uint16_t color_curr_bar = 0x7A7D;
	// uint16_t x_bar = 80;
	// uint16_t y_bar = 10;
	// uint16_t k_bar = 20;

	switch (menu_page)
	{
	case 1:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar-5, y_bar, menubar1_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("STETIG 0/2-10V", x_text - 10, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   home_menu_66x66, 66, 66, color_block, sizeof home_menu_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, antrieb_r_80x80, 80,
							   80, color_curr_block, sizeof antrieb_r_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   antrieb_s_66x66, 66, 66, color_block, sizeof antrieb_s_66x66);
		break;
	case 2:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*2-29, y_bar, menubar2_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("AUF/ZU 3-PUNKT", x_text - 10, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   antrieb_r_66x66, 66, 66, color_block, sizeof antrieb_r_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, antrieb_s_80x80, 80,
							   80, color_curr_block, sizeof antrieb_s_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   home_menu_66x66, 66, 66, color_block, sizeof home_menu_66x66);
		break;
	case 3:															 //
		ILI9341_fillRect(x_block_left, 54, 310, 180, COLOR_MENU_BG); //
		// ILI9341_drawBitmapFast(x_block_left+25, 10, menubar_320x34, 320, 34, color_bar);
		// ILI9341_drawBitmapFast(x_bar*2-29, y_bar, menubar2_80x34, 80, 34, color_curr_bar);
		ILI9341_printText("HOME", x_text + 50, y_text, COLOR_WHITE,
						  COLOR_MENU_BG, 2);

		ILI9341_drawBitmapFast(x_block_left, y_block_left_right,
							   antrieb_s_66x66, 66, 66, color_block, sizeof antrieb_s_66x66);
		ILI9341_drawBitmapFast(x_block_mid, y_block_mid, home_menu_80x80, 80,
							   80, color_curr_block, sizeof home_menu_80x80);
		ILI9341_drawBitmapFast(x_block_right, y_block_left_right,
							   antrieb_r_66x66, 66, 66, color_block, sizeof antrieb_r_66x66);
		// HAL_Delay(delay_menus);
		// ILI9341_Delay(del_menu);
		break;
	}
}
//-------------------------------------------------------------------
void draw_antriebAnim(uint16_t x, uint16_t y, int16_t val)
{ // x=170,y=80
	uint16_t color_on = COLOR_GREEN;
	uint16_t color_off = COLOR_RED;
	uint16_t ind_lenght = 100;
	uint16_t ind_width = 10;
	if (val < 101)
	{
		if (val != valOld)
		{
			valOld = val;
			ILI9341_fillRect(x, y + ind_lenght, x + ind_width, y, color_off);
			ILI9341_fillRect(x, y + ind_lenght, x + ind_width,
							 y + ind_lenght - val, color_on);
		}
	}
}
