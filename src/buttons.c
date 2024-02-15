/*
 * buttons.c
 *
 *  Created on: 22.07.2021
 *      Author: Alex
 */

#include "spi_ili9341.h"
#include "menu.h"
#include "main.h"
#include "buttons.h"
#include "w25qxx.h"
#include "processing.h"
//#include "processing.h"
//#include "pic.c"

extern DAC_HandleTypeDef hdac; //for adc
//extern uint16_t DacVal;

extern uint8_t page_main_menu;
extern uint8_t page_sensor_menu;
extern uint8_t page_antrieb_menu;
extern uint8_t page_info_menu;

extern uint8_t current_main_page; //read ADC
extern uint8_t current_sensor_page; //read ADC
extern uint8_t current_antrieb_page; //read ADC
extern uint8_t current_info_page; //read ADC
extern uint8_t current_pt100_page;

extern uint8_t measuring_range_raumregler_aktiv;
extern uint8_t measuring_range_raumtemp;
extern uint8_t measuring_range_kanaltemp;
extern uint8_t measuring_range_feuchte;
extern uint8_t measuring_range_druckluft;
extern uint8_t measuring_range_helligkeit;
extern uint16_t cy_old;

extern uint16_t frame_val_ystart;
extern uint16_t frame_val_ysize;
extern uint16_t frame_val_xstart;

extern uint16_t x_pos_button_l;
extern uint16_t y_pos_button_l;
extern uint16_t x_pos_button_r;
extern uint16_t y_pos_button_r;
extern uint16_t x_pos_button_c;
extern uint16_t y_pos_button_c;

extern uint16_t cy_old; //cy_oldcl for pt100 graphic
//values for selfbuild sensor
extern int16_t yvalue_min;
extern uint16_t yvalue_max;
extern uint16_t multiply_min_max;
//---
//game
extern bool button_left_pressed;
extern bool button_middle_pressed;
extern bool button_right_pressed;
//game

extern bool running; //check while doing
char str[10];
uint8_t test_menu = 0;
bool button_pressed = false;
uint16_t buttons_delay = 400; // butt del mili
uint8_t buttons_l_r_status = 0; // butt left/right status
uint8_t button_select_status = 1; // butt select/home status
uint8_t buttons_l_r_status_settings = 0; // butt left/right status in settings
uint8_t buttons_up_down_status = 0; // butt left/right status in settings
uint32_t cnt_new_buttons = 0; //cnt for buttons
uint32_t cnt_old_buttons = 0;
uint8_t DacVolt = 0;
uint8_t secret_menu_cnt = 0;

//---------------------ini values for menu settings

uint8_t flash_write_buffer1[6];
//uint8_t flash_write_buffer2[8] = {1,2,3,4,5,6,7,8};
uint8_t flash_read_buffer1[6];
//uint8_t flash_read_buffer2[8];

//for FLASH

uint32_t cnt_idle = 0;

bool status_idle = false;
//set para

uint8_t standart_helligkeit_choice;
uint8_t standart_helligkeit;

uint8_t time_30helligkeit_choice;
uint16_t time1_idle; //in sec

uint8_t time_0helligkeit_choice;
uint16_t time2_idle; //in sec

uint8_t time_24VOFF_choice;
uint16_t time3_idle; //in sec

uint8_t touch_ton_choice;
uint8_t touch_ton;

uint8_t alarm_ton_choice;
uint8_t alarm_ton;

void buttons_enable(uint8_t status) {
	if (status == 0) {
		HAL_GPIO_WritePin(GPIOC, TOUCH_Pin, GPIO_PIN_SET);
	}
	if (status == 1) {
		HAL_GPIO_WritePin(GPIOC, TOUCH_Pin, GPIO_PIN_RESET);
	}
}
void interrupts_enable(uint8_t status) {
	if (status == 0) {
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
	}
	if (status == 1) {
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}

void button_up() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);
		switch (buttons_l_r_status) {
		case 7: // read_adc_info up
//			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
//			COLOR_MENU_BAR_DOWN, "<<");
			draw_infoMenuUp();
			test_menu = 1; //read adc all channels
			break;
		case 8: // read_adc_ybutton min+
			draw_button(x_pos_button_r, y_pos_button_r - 43, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "*10");
			multiply_min_max *= 10;
			if (multiply_min_max == 10000) {
				multiply_min_max = 1;
			}
			break;
		case 9: // read_adc_ybutton max+
			draw_button(x_pos_button_r, y_pos_button_r - 43, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "*10");
			multiply_min_max *= 10;
			if (multiply_min_max == 10000) {
				multiply_min_max = 1;
			}
			break;
		}
		switch (buttons_up_down_status) {
		case 1:
			buttons_l_r_status_settings--;
			if (buttons_l_r_status_settings == 0) {
				buttons_l_r_status_settings = 1;
			}
			break;
		}
		DelayMili(buttons_delay);
		button_pressed = false;
		//buttons_enable(on);
	}
	cnt_idle = 0;
}
void button_left() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);
		switch (button_select_status) {
		case 1:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			page_main_menu--;
			if (page_main_menu == 0) {
				page_main_menu = 4;
			}


			select_main_menu(page_main_menu);

			break;
		case 2:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			page_sensor_menu--;
			if (page_sensor_menu == 0) {
				page_sensor_menu = 11;
			}
			select_sensor_submenu(page_sensor_menu);

			break;
		case 3:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			page_antrieb_menu--;
			if (page_antrieb_menu == 0) {
				page_antrieb_menu = 3;
			}
//			sprintf(str, "%3d", page_antrieb_menu);
//			ILI9341_printText(str, 10, 40, COLOR_WHITE, COLOR_MENU_BG, 2);
			select_antrieb_submenu(page_antrieb_menu);
			break;
		}
		switch (buttons_l_r_status) {
		case 1: // read_menu_antrieb_r y-
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "Y-");
			DacVolt--;
			if (DacVolt == 0) {
				DacVolt = 0;
			}
			//dac_1v(DacVolt);

			//for(uint32_t i = 0;i<500000;i++){}
			break;
		case 2: //read_raumregler_aktiv m-
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M-");
			measuring_range_raumregler_aktiv--;
			if (measuring_range_raumregler_aktiv == 0) {
				measuring_range_raumregler_aktiv = 3;
			}

			break;
//		case 3: // read_adc_raumtemp m-
//			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
//			COLOR_MENU_BAR_DOWN, "M-");
//			measuring_range_raumtemp--;
//			if (measuring_range_raumtemp == 0) {
//				measuring_range_raumtemp = 3;
//			}
//
//			break;
		case 3: // read_adc_kanaltemp m-
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M-");
			measuring_range_kanaltemp--;
			if (measuring_range_kanaltemp == 0) {
				measuring_range_kanaltemp = 8;
			}

			break;
		case 4: // read_adc_feuchte m-
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M-");
			measuring_range_feuchte--;
			if (measuring_range_feuchte == 0) {
				measuring_range_feuchte = 4;
			}

			break;
		case 5: // read_adc_druckluft m-
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M-");
			measuring_range_druckluft--;
			if (measuring_range_druckluft == 0) {
				measuring_range_druckluft = 13;
			}

			break;
		case 6: // read_adc_helligkeit m-
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M-");
			measuring_range_helligkeit--;
			if (measuring_range_helligkeit == 0) {
				measuring_range_helligkeit = 6;
			}

			break;
		case 7: // read_adc_info p-
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			draw_infoMenu1();
			break;
		case 8: // read_adc_ybutton min -
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "- ");
			yvalue_min -= multiply_min_max; //
			//if(yvalue_min <= -2500){yvalue_max = 2500;}

			break;
		case 9: // read_adc_ybutton max -
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "- ");
			yvalue_max -= multiply_min_max;
			//if(yvalue_max <= 0){yvalue_max = 1;}

			break;
		case 10: // AUF/ZU menu
			//				draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			//				COLOR_MENU_BAR_DOWN, "INFO");
			//				draw_menu_antrieb_s_info();
			//
			break;
		case 11: //pt100 graphic
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, "min");
			current_pt100_page = 1;
			draw_graphic_m(30, 20, COLOR_WHITE);
			cy_old = 0;
			break;

		}		//switch
		switch (buttons_l_r_status_settings) {
		case 1:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			standart_helligkeit_choice--;
			if (standart_helligkeit_choice == 0) {
				standart_helligkeit_choice = 1;
			}

			break;
		case 2:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			time_30helligkeit_choice--;
			if (time_30helligkeit_choice == 0) {
				time_30helligkeit_choice = 1;
			}
			break;
		case 3:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			time_0helligkeit_choice--;
			if (time_0helligkeit_choice == 0) {
				time_0helligkeit_choice = 1;
			}
			break;
		case 4:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			time_24VOFF_choice--;
			if (time_24VOFF_choice == 0) {
				time_24VOFF_choice = 1;
			}
			break;
		case 5:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			touch_ton_choice--;
			if (touch_ton_choice == 0) {
				touch_ton_choice = 1;
			}
			start_buzzer(touch_ton_choice);
			break;
		case 6:
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "<<");
			alarm_ton_choice--;
			if (alarm_ton_choice == 0) {
				alarm_ton_choice = 1;
			}
			break;
		}
		DelayMili(buttons_delay);
		button_pressed = false;
	}
	cnt_idle = 0;
}
void button_middle() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);
		;
		switch (button_select_status) {
		case 1: // draw submenu
			//buttons_enable(off);
			draw_start_button(COLOR_BLACK);
			current_sensor_page = 0;
			current_antrieb_page = 0;
			buttons_l_r_status = 0;
			current_main_page = 1;
			//DelayMili(buttons_delay);
			draw_main_menu(page_main_menu);

			//buttons_enable(on);
			break;
		case 2: // draw sensor menu
			//buttons_enable(off);
			draw_start_button(COLOR_BLACK);
			current_sensor_page = page_sensor_menu;
			current_main_page = 0;

			draw_sensor_submenu(page_sensor_menu);

			//buttons_enable(on);
			break;
		case 3: // draw antrieb menu
			//buttons_enable(off);
			draw_start_button(COLOR_BLACK);
			current_antrieb_page = page_antrieb_menu;
			current_main_page = 0;
			//DelayMili(buttons_delay);
			draw_antrieb_submenu(page_antrieb_menu);

			//buttons_enable(on);
			break;
		case 4: // draw home from selected menu
			//running = false;
			//buttons_enable(off);
			draw_start_button(COLOR_BLACK);
			current_sensor_page = 0;
			current_antrieb_page = 0;
			current_main_page = 1;
			buttons_l_r_status = 0;
			button_select_status = 1;
			buttons_l_r_status_settings = 0;
			buttons_up_down_status = 0;
			yvalue_min = 1;
			yvalue_max = 1;
			multiply_min_max = 1;
			test_menu = 0;
			draw_menu_home();
			//return;
			break;
		case 5: // select measuring range for ysensor min ok
			//buttons_enable(off);
			draw_start_button(COLOR_BLACK);
			draw_m_frame(frame_val_xstart - 90, frame_val_ystart);
			draw_s_frame(frame_val_xstart - 90, frame_val_ystart * 2 - 4);
			buttons_l_r_status = 9;
			button_select_status = 6;
			draw_start_button(COLOR_WHITE);
			//buttons_enable(on);
			break;
		case 6: // select measuring range for ysensor max
			//buttons_enable(off);
			draw_start_button(COLOR_BLACK);
			current_sensor_page = 13;
			button_select_status = 4;
			buttons_l_r_status = 0;
			draw_menu_ysensor();
			//buttons_enable(on);
			break;
		case 7: // game
			draw_start_button(COLOR_BLACK);
			//current_sensor_page = 0;
			//current_antrieb_page = 0;
			current_main_page = 1;
			//buttons_l_r_status = 0;
			button_select_status = 1;
			buttons_l_r_status_settings = 0;
			buttons_up_down_status = 0;
			//yvalue_min = 1;
			//yvalue_max = 1;
			//multiply_min_max = 1;
			test_menu = 0;
			write_flash_settings();
			draw_menu_home();
			break;
		}
		DelayMili(buttons_delay);
		button_pressed = false;
		//buttons_enable(on);
	}
	cnt_idle = 0;
}
void button_right() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);

		switch (button_select_status) {
		case 1:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, ">>");
			page_main_menu++;
			if (page_main_menu == 5) {
				page_main_menu = 1;
			}
			select_main_menu(page_main_menu);
			//DelayMili(buttons_delay);
			//return;
			break;
		case 2:
			page_sensor_menu++;
			if (page_sensor_menu == 12) {
				page_sensor_menu = 1;
			}
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, ">>");
			select_sensor_submenu(page_sensor_menu);
			//DelayMili(buttons_delay);
			break;
		case 3:
			page_antrieb_menu++;
			if (page_antrieb_menu == 4) {
				page_antrieb_menu = 1;
			}
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, ">>");
			select_antrieb_submenu(page_antrieb_menu);
			//DelayMili(buttons_delay);
			break;
		}
		switch (buttons_l_r_status) {
		case 1: // read_menu_antrieb_r y+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "Y+");
			DacVolt++;
			if (DacVolt == 13) {
				DacVolt = 12;
			}
			//dac_1v(DacVolt);
			//DelayMili(buttons_delay);
			break;
		case 2: //read_raumregler_aktiv m+
			draw_button(x_pos_button_r, y_pos_button_l, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M+");
			measuring_range_raumregler_aktiv++;
			if (measuring_range_raumregler_aktiv == 4) {
				measuring_range_raumregler_aktiv = 1;
			}
			//DelayMili(buttons_delay);
			break;
//		case 3: // read_adc_raumtemp m+
//			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
//			COLOR_MENU_BAR_DOWN, "M+");
//			measuring_range_raumtemp++;
//			if (measuring_range_raumtemp == 4) {
//				measuring_range_raumtemp = 1;
//			}
//			//DelayMili(buttons_delay);
//			break;
		case 3: // read_adc_kanaltemp m+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M+");
			measuring_range_kanaltemp++;
			if (measuring_range_kanaltemp == 9) {
				measuring_range_kanaltemp = 1;
			}
			//DelayMili(buttons_delay);
			break;
		case 4: // read_adc_feuchte m+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M+");
			measuring_range_feuchte++;
			if (measuring_range_feuchte == 5) {
				measuring_range_feuchte = 1;
			}
			//DelayMili(buttons_delay);
			break;
		case 5: // read_adc_druckluft m+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M+");
			measuring_range_druckluft++;
			if (measuring_range_druckluft == 14) {
				measuring_range_druckluft = 1;
			}
			//DelayMili(buttons_delay);
			break;
		case 6: // read_adc_helligkeit m+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "M+");
			measuring_range_helligkeit++;
			if (measuring_range_helligkeit == 7) {
				measuring_range_helligkeit = 1;
			}
			//DelayMili(buttons_delay);
			break;
		case 7: // read_adc_info p+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, ">>");
			draw_infoMenu2();
//			secret_menu_cnt++;
//			sprintf(str, "%1d", secret_menu_cnt);
//			ILI9341_printText(str, 205, 215, COLOR_WHITE, 0xe9, 1);
//			if (secret_menu_cnt > 2) {
//				buttons_enable(on);
//				button_select_status = 7;
//				buttons_l_r_status = 13;
//				secret_menu_cnt = 0;
//			}
			break;
		case 8: // read_adc_ybutton min+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, " +");
			yvalue_min += multiply_min_max;
//			if (yvalue_min >= 9999) {
//				yvalue_min = 9999;
//			}
			//DelayMili(buttons_delay);
			break;
		case 9: // read_adc_ybutton max+
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, " +");
			yvalue_max += multiply_min_max;
			//if(yvalue_max >= 50000){yvalue_max = 50000;}

			break;
		case 10: // AUF/ZU menu
			//				draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			//								COLOR_MENU_BAR_DOWN, "<<<");
			//				draw_menu_antrieb_s();
			//				DelayMili(buttons_delay);
			break;
		case 11:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, "std");
			draw_graphic_m(30, 20, COLOR_WHITE);
			ILI9341_string(225, 185, "10 STD", COLOR_WHITE, COLOR_MENU_BG);
			current_pt100_page = 2;
			cy_old = 0;

			break;
		}
		switch (buttons_l_r_status_settings) {
		case 1:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, ">>");
			standart_helligkeit_choice++;
			if (standart_helligkeit_choice == 5) {
				standart_helligkeit_choice = 4;
			}
			break;
		case 2:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, ">>");
			time_30helligkeit_choice++;
			if (time_30helligkeit_choice == 5) {
				time_30helligkeit_choice = 4;
			}
			break;
		case 3:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, ">>");
			time_0helligkeit_choice++;
			if (time_0helligkeit_choice == 5) {
				time_0helligkeit_choice = 4;
			}
			break;
		case 4:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, ">>");
			time_24VOFF_choice++;
			if (time_24VOFF_choice == 5) {
				time_24VOFF_choice = 4;
			}
			break;
		case 5:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, ">>");
			touch_ton_choice++;
			if (touch_ton_choice == 5) {
				touch_ton_choice = 4;
			}
			break;
		case 6:
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
			COLOR_MENU_BAR_DOWN, ">>");
			alarm_ton_choice++;
			if (alarm_ton_choice == 5) {
				alarm_ton_choice = 4;
			}
			break;
		}
		DelayMili(buttons_delay);
		button_pressed = false;
	}
	cnt_idle = 0;
}
void button_down() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);
		switch (buttons_l_r_status) {
		case 8: // read_adc_ybutton min-
			draw_button(x_pos_button_l, y_pos_button_l - 43, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "/10");
			multiply_min_max /= 10;
			if (multiply_min_max == 0) {
				multiply_min_max = 1;
			}
			break;
		case 9: // read_adc_ybutton max-
			draw_button(x_pos_button_l, y_pos_button_l - 43, COLOR_BLACK,
			COLOR_MENU_BAR_DOWN, "/10");
			multiply_min_max /= 10;
			if (multiply_min_max == 0) {
				multiply_min_max = 1;
			}

			break;

		}
		switch (buttons_up_down_status) {
		case 1:
			buttons_l_r_status_settings++;
			if (buttons_l_r_status_settings == 7) {
				buttons_l_r_status_settings = 6;
			}
			break;
		}
		DelayMili(buttons_delay);
		button_pressed = false;
		//buttons_enable(on);
	}
	cnt_idle = 0;
}
void button_fun1() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);

		switch (buttons_l_r_status) {
		case 1: // read_menu_antrieb_r y--
			draw_button(x_pos_button_l, y_pos_button_l, COLOR_BLUE,
			COLOR_MENU_BAR_DOWN, "--");
			DacVolt = 0;
			break;

//		case 13: //game
//			draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
//			COLOR_MENU_BAR_DOWN, "<<");
//			button_left_pressed = true;
//			break;

		}
		button_pressed = false;
		//buttons_enable(on);
	} //if running
	cnt_idle = 0;
}
void button_fun2() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);
		switch (buttons_l_r_status) {
		case 1: // read_menu_antrieb_r y--
			draw_start_button(COLOR_YELLOW);
			DacVolt = 5;
			draw_start_button(COLOR_WHITE);
			break;
		}
//		switch (button_select_status) {
//		case 7: // game
//			draw_start_button(COLOR_YELLOW);
//			button_middle_pressed = true;
//			break;
//		}
		button_pressed = false;
		//buttons_enable(on);
	} //if running
	cnt_idle = 0;
}
void button_fun3() {
	if (running == false) {
		button_pressed = true;
		//buttons_enable(off);
		start_buzzer(touch_ton);
		start_vibration();
		start_buzzer(0);
		switch (buttons_l_r_status) {
		case 1: // read_menu_antrieb_r y--
			draw_button(x_pos_button_r, y_pos_button_r, COLOR_RED,
			COLOR_MENU_BAR_DOWN, "++");
			DacVolt = 10;
			break;
//		case 13: //game
//			draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
//			COLOR_MENU_BAR_DOWN, ">>");
//			button_right_pressed = true;
//			break;
		}
		button_pressed = false;
		//buttons_enable(on);
	} //if running
	cnt_idle = 0;
}
void start_vibration() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
	DelayMili(60);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
}
void draw_main_menu(uint8_t draw_page) { //for it buttons
//HAL_GPIO_WritePin(GPIOE, V24ON1_Pin, GPIO_PIN_RESET);
//HAL_GPIO_WritePin(GPIOE, V24ON2_Pin, GPIO_PIN_RESET);
	switch (draw_page) {
	case 1: //
		button_select_status = 2;
		select_sensor_submenu(page_sensor_menu);
		break;
	case 2: //
		button_select_status = 3;
		select_antrieb_submenu(page_antrieb_menu);
		break;
	case 3: //
		button_select_status = 4;
		buttons_l_r_status = 7;
		current_main_page = 2;
		draw_infoMenu1();
		break;
	case 4: //
		button_select_status = 7;
		buttons_l_r_status = 0;
		buttons_l_r_status_settings = 1;
		buttons_up_down_status = 1;
		current_main_page = 3;
		draw_menu_tools();
		break;
	}
}
//-------------------------------------------------------------------
void draw_sensor_submenu(uint8_t draw_page) { //for it buttons
//HAL_GPIO_WritePin(GPIOE, V24ON1_Pin, GPIO_PIN_RESET);
//HAL_GPIO_WritePin(GPIOE, V24ON2_Pin, GPIO_PIN_RESET);
	switch (draw_page) {
	case 0: //
		break;
	case 1: //
		button_select_status = 4;
		buttons_l_r_status = 2;
		draw_menu_raumregler_aktiv();
		break;
	case 2: //
		button_select_status = 4;
		draw_menu_raumregler_passiv();
		break;
//	case 3: //
//		button_select_status = 4;
//		buttons_l_r_status = 3;
//		draw_menu_raumtemp();
//		break;
	case 3: //
		button_select_status = 4;
		buttons_l_r_status = 3;
		draw_menu_kanaltemp();
		break;
	case 4: //
		button_select_status = 4;
		buttons_l_r_status = 4;
		draw_menu_feuchte();
		break;
	case 5: //
		button_select_status = 4;
		draw_menu_druckwasser();
		break;
	case 6: //
		button_select_status = 4;
		buttons_l_r_status = 5;
		draw_menu_druckluft();
		break;
	case 7: //
		button_select_status = 4;
		draw_menu_luftquali();
		break;
	case 8: //
		button_select_status = 4;
		buttons_l_r_status = 6;
		draw_menu_helligkeit();
		break;
	case 9: //
		button_select_status = 4;
		buttons_l_r_status = 11;
		draw_pt100menu();
		break;
	case 10: //
		buttons_l_r_status = 8;
		button_select_status = 5;
		draw_premenu_ysensor();
		break;
	case 11: //home
		buttons_l_r_status = 0;
		button_select_status = 1;
		current_main_page = 1;
		draw_menu_home();
		break;
	}
}
//-------------------------------------------------------------------
void draw_antrieb_submenu(uint8_t draw_page) { //for it buttons
//HAL_GPIO_WritePin(GPIOE, V24ON1_Pin, GPIO_PIN_RESET);
//HAL_GPIO_WritePin(GPIOE, V24ON2_Pin, GPIO_PIN_RESET);
	switch (draw_page) {
	case 0: //
		break;
	case 1: //
		button_select_status = 4;
		buttons_l_r_status = 1;
		draw_menu_antrieb_r();
		break;
	case 2: //
		button_select_status = 4;
		//buttons_l_r_status = 11;
		draw_menu_antrieb_s();
		break;
	case 3: //
		buttons_l_r_status = 0;
		button_select_status = 1;
		current_main_page = 1;
		draw_menu_home();
		break;
	}
}
//-------------------------------------------------------------------
void idle() {
	//-----timer for display off
	cnt_idle++;
	if (cnt_idle > time1_idle * 2) { //5min  1 tick 0,5 sec
		if (standart_helligkeit < 25) {
			ILI9341_SetBrightness(10);
		} else {
			ILI9341_SetBrightness(25);
		}
	} else {
		status_idle = true;
		ILI9341_SetBrightness(standart_helligkeit);
	}
	if (cnt_idle > time2_idle * 2) { //10min  1 tick 0,5 sec
		ILI9341_SetBrightness(1);
	}
	if (cnt_idle > time3_idle * 2) { //1std. 1 tick 0,5 sec
		ILI9341_SetBrightness(1);
		status_idle = false;
		HAL_GPIO_WritePin(GPIOB, V24ON1_Pin, GPIO_PIN_RESET); //24VINTERN
		HAL_GPIO_WritePin(GPIOB, V24ON2_Pin, GPIO_PIN_RESET); //24VOUT
	}
}
void ini_buttons() {
	//W25qxx_EraseSector(1);
	//W25qxx_WriteSector(buffer1, 1, 0, 8);
	W25qxx_ReadSector(flash_read_buffer1, 1, 0, 6);
	//HAL_Delay(100);
	standart_helligkeit_choice = flash_read_buffer1[0]; //flash_read_buffer1[0]
	time_30helligkeit_choice = flash_read_buffer1[1];  //flash_read_buffer1[1]
	time_0helligkeit_choice = flash_read_buffer1[2];   //flash_read_buffer1[2]
	time_24VOFF_choice = flash_read_buffer1[3];       //flash_read_buffer1[3]
	touch_ton_choice = flash_read_buffer1[4];          //flash_read_buffer1[4]
	alarm_ton_choice = flash_read_buffer1[5];         //flash_read_buffer1[5]
//	standart_helligkeit_choice = 4; //flash_read_buffer1[0]
//	time_30helligkeit_choice = 1;  //flash_read_buffer1[1]
//	time_0helligkeit_choice = 1;   //flash_read_buffer1[2]
//	time_24VOFF_choice = 2;       //flash_read_buffer1[3]
//	touch_ton_choice = 1;          //flash_read_buffer1[4]
//	alarm_ton_choice = 1;         //flash_read_buffer1[5]
	set_settings();
}
void set_settings() {

	switch (standart_helligkeit_choice) {         //1
//	case 0:
//		standart_helligkeit = 0;
//		break;
	case 1:
		standart_helligkeit = 10;
		break;
	case 2:
		standart_helligkeit = 25;
		break;
	case 3:
		standart_helligkeit = 60;
		break;
	case 4:
		standart_helligkeit = 100;
		break;
	default:
		standart_helligkeit = 100;
		break;
	}
	switch (time_30helligkeit_choice) {         //2
	case 1:
		time1_idle = 360;  //5 min
		break;
	case 2:
		time1_idle = 720;  //10 min
		break;
	case 3:
		time1_idle = 1440; //20 min
		break;
	case 4:
		time1_idle = 32000; //off
		break;
	default:
		time1_idle = 360;
		break;
	}
	switch (time_0helligkeit_choice) { //3
	case 1:
		time2_idle = 720;  //10 min
		break;
	case 2:
		time2_idle = 1440;  //20 min
		break;
	case 3:
		time2_idle = 1800; //40 min
		break;
	case 4:
		time2_idle = 32000; //off
		break;
	default:
		time2_idle = 720;
		break;
	}
	switch (time_24VOFF_choice) { //4
	case 1:
		time3_idle = 720;  //10 min
		break;
	case 2:
		time3_idle = 1750;  //30 min
		break;
	case 3:
		time3_idle = 3600; //60 min
		break;
	case 4:
		time3_idle = 32000; //off
		break;
	default:
		time3_idle = 3600;
		break;
	}

	switch (touch_ton_choice) { //5
//	case 0:
//		touch_ton = 0;
//		break;
	case 1:
		touch_ton = 1;
		break;
	case 2:
		touch_ton = 2;
		break;
	case 3:
		touch_ton = 3;
		break;
	case 4:
		touch_ton = 4;
		break;
	default:
		touch_ton = 1;
		break;
	}
	switch (alarm_ton_choice) {  //6
	case 1:
		alarm_ton = 1;
		break;
	case 2:
		alarm_ton = 2;
		break;
	case 3:
		alarm_ton = 3;
		break;
	case 4:
		alarm_ton = 4;
		break;
	default:
		alarm_ton = 2;
		break;
	}
}
void update_tools_menu() {
	draw_button(x_pos_button_l, y_pos_button_l, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, "<<");
	draw_button(x_pos_button_r, y_pos_button_r, COLOR_WHITE,
	COLOR_MENU_BAR_DOWN, ">>");
	draw_start_button(COLOR_WHITE);
///1
	sprintf(str, "%1d", standart_helligkeit_choice);
	ILI9341_printText(str, 200, 60,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	sprintf(str, "%4d%%", standart_helligkeit);
	ILI9341_printText(str, 230, 60,
	COLOR_WHITE, COLOR_MENU_BG, 2);
///2
	sprintf(str, "%1d", time_30helligkeit_choice);
	ILI9341_printText(str, 200, 80,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	sprintf(str, "%4ds", time1_idle);
	ILI9341_printText(str, 230, 80,
	COLOR_WHITE, COLOR_MENU_BG, 2);
///3
	sprintf(str, "%1d", time_0helligkeit_choice);
	ILI9341_printText(str, 200, 100,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	sprintf(str, "%4ds", time2_idle);
	ILI9341_printText(str, 230, 100,
	COLOR_WHITE, COLOR_MENU_BG, 2);
///4
	sprintf(str, "%1d", time_24VOFF_choice);
	ILI9341_printText(str, 200, 120,
	COLOR_WHITE, COLOR_MENU_BG, 2);
	sprintf(str, "%4ds", time3_idle);
	ILI9341_printText(str, 230, 120,
	COLOR_WHITE, COLOR_MENU_BG, 2);
///5
	sprintf(str, "%1d", touch_ton_choice);
	ILI9341_printText(str, 200, 140,
	COLOR_WHITE, COLOR_MENU_BG, 2);

///6
	sprintf(str, "%1d", alarm_ton_choice);
	ILI9341_printText(str, 200, 160,
	COLOR_WHITE, COLOR_MENU_BG, 2);

	switch (buttons_l_r_status_settings) {
	case 1:
		sprintf(str, "%1d", standart_helligkeit_choice);
		ILI9341_printText(str, 200, 60,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		sprintf(str, "%4d%%", standart_helligkeit);
		ILI9341_printText(str, 230, 60,
		COLOR_WHITE, COLOR_MENU_BG, 2);
		break;
	case 2:
		sprintf(str, "%1d", time_30helligkeit_choice);
		ILI9341_printText(str, 200, 80,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		sprintf(str, "%4ds", time1_idle);
		ILI9341_printText(str, 230, 80,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		break;
	case 3:
		sprintf(str, "%1d", time_0helligkeit_choice);
		ILI9341_printText(str, 200, 100,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		sprintf(str, "%4ds", time2_idle);
		ILI9341_printText(str, 230, 100,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		break;
	case 4:
		sprintf(str, "%1d", time_24VOFF_choice);
		ILI9341_printText(str, 200, 120,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		sprintf(str, "%4ds", time3_idle);
		ILI9341_printText(str, 230, 120,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		break;
	case 5:
		sprintf(str, "%1d", touch_ton_choice);
		ILI9341_printText(str, 200, 140,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		break;
	case 6:
		sprintf(str, "%1d", alarm_ton_choice);
		ILI9341_printText(str, 200, 160,
		COLOR_GREEN, COLOR_MENU_BG, 2);
		break;
	}
	set_settings();

	//W25qxx_ReadSector(flash_read_buffer1, 1, 0, 7);
	//HAL_Delay(100);

//running = false;
}
void write_flash_settings() {
	flash_write_buffer1[0] = standart_helligkeit_choice; //flash_read_buffer1[0]
	flash_write_buffer1[1] = time_30helligkeit_choice;  //flash_read_buffer1[1]
	flash_write_buffer1[2] = time_0helligkeit_choice;   //flash_read_buffer1[2]
	flash_write_buffer1[3] = time_24VOFF_choice;       //flash_read_buffer1[3]
	flash_write_buffer1[4] = touch_ton_choice;          //flash_read_buffer1[4]
	flash_write_buffer1[5] = alarm_ton_choice;         //flash_read_buffer1[5]
	W25qxx_EraseSector(1);
	W25qxx_WriteSector(flash_write_buffer1, 1, 0, 6);
}
