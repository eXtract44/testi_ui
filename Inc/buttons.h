/*
 * buttons.h
 *
 *  Created on: 22.07.2021
 *      Author: Alex
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_


void button_up();
void button_left();
void button_middle();
void button_right();
void button_down();
void button_fun1();
void button_fun2();
void button_fun3();
void start_vibration();
void draw_main_menu(uint8_t draw_page);
void draw_sensor_submenu(uint8_t draw_page);
void draw_antrieb_submenu(uint8_t draw_page);
void update_tools_menu();
void set_settings();
void write_flash_settings();
void ini_buttons();
void buttons_enable(uint8_t status);
void idle();
#endif /* INC_BUTTONS_H_ */
